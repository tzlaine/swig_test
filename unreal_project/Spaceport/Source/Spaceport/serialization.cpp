#include "serialization.hpp"

#include "model.hpp"

#include <boost/container/small_vector.hpp>


namespace detail {
    void serialize_for_client(
        game_state_t const & gs,
        std::vector<fleet_t const *> const & visible_fleets,
        int nation_id,
        unit_design_t const & x,
        visibility_kind vis,
        ostream_tarray_facade * os)
    {
        if (vis == visibility_kind::owner) {
            serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 0, os);
        } else if (vis == visibility_kind::neutral_or_enemy) {
            std::array<int, 10> fields_to_elide = {
                {metadata<unit_design_t>::armor().index_,
                 metadata<unit_design_t>::propulsion().index_,
                 metadata<unit_design_t>::weapons().index_,
                 metadata<unit_design_t>::shields().index_,
                 metadata<unit_design_t>::detection().index_,
                 metadata<unit_design_t>::stealth().index_,
                 metadata<unit_design_t>::automation().index_,
                 metadata<unit_design_t>::attack().index_,
                 metadata<unit_design_t>::defense().index_,
                 metadata<unit_design_t>::ground_attack().index_}};
            serialize_message_impl<ser_op::write, ser_field_op::dont_write>(
                x, 0, os, fields_to_elide);
        } else {
            detail::serialize_message_end<ser_op::write>(os);
        }
    }

    void serialize_for_client(
        game_state_t const & gs,
        std::vector<fleet_t const *> const & visible_fleets,
        int nation_id,
        unit_t const & x,
        visibility_kind vis,
        ostream_tarray_facade * os)
    {
        if (vis == visibility_kind::owner) {
            serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 0, os);
        } else if (vis == visibility_kind::neutral_or_enemy) {
            std::array<int, 1> fields_to_elide = {
                {metadata<unit_t>::health().index_}};
            serialize_message_impl<ser_op::write, ser_field_op::dont_write>(
                x, 0, os, fields_to_elide);
        } else {
            detail::serialize_message_end<ser_op::write>(os);
        }
    }

    void serialize_for_client(
        game_state_t const & gs,
        std::vector<fleet_t const *> const & visible_fleets,
        int nation_id,
        fleet_t const & x,
        visibility_kind vis,
        ostream_tarray_facade * os)
    {
        if (vis == visibility_kind::owner) {
            serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 0, os);
        } else if (vis == visibility_kind::neutral_or_enemy) {
            fleet_t copy = x;
            for (auto & unit : copy.units) {
                unit.health = -1;
            }
            std::array<int, 5> fields_to_elide = {
                {metadata<fleet_t>::mission().index_,
                 metadata<fleet_t>::fuel().index_,
                 metadata<fleet_t>::rounds().index_,
                 metadata<fleet_t>::missiles().index_,
                 metadata<fleet_t>::fighters().index_}};
            serialize_message_impl<ser_op::write, ser_field_op::dont_write>(
                copy, 0, os, fields_to_elide);
        } else {
            detail::serialize_message_end<ser_op::write>(os);
        }
    }

    void serialize_for_client(
        game_state_t const & gs,
        std::vector<fleet_t const *> const & visible_fleets,
        int nation_id,
        hex_t const & x,
        visibility_kind vis,
        int,
        ostream_tarray_facade * os)
    {
        if (vis == visibility_kind::owner ||
            vis == visibility_kind::neutral_or_enemy) {
            serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 0, os);
        } else {
            detail::serialize_message_end<ser_op::write>(os);
        }
    }

    void serialize_for_client(
        game_state_t const & gs,
        std::vector<fleet_t const *> const & visible_fleets,
        int nation_id,
        system_t const & x,
        visibility_kind vis,
        int system_index,
        ostream_tarray_facade * os)
    {
        if (vis == visibility_kind::owner) {
            serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 0, os);
        } else if (vis == visibility_kind::neutral_or_enemy) {
            std::array<int, 4> fields_to_elide = {
                {metadata<system_t>::permanent_locations().index_,
                 metadata<system_t>::temporary_locations().index_,
                 metadata<system_t>::first_planet().index_,
                 metadata<system_t>::last_planet().index_}};
            serialize_message_impl<ser_op::write, ser_field_op::dont_write>(
                x, 0, os, fields_to_elide);
        } else {
            detail::serialize_message_end<ser_op::write>(os);
        }
    }

    // std::ranges::set_intersection() is tantalizingly close to what we need
    // below for filtering planet_t::settlement_id.  However, our use below
    // does not satisfy the mergeable concept.  This is similar, but slightly
    // simplified, and with the constraints stripped off.
    template<
        typename R1,
        typename R2,
        typename O,
        typename Cmp,
        typename Proj1,
        typename Proj2>
    O set_intersection(
        R1 && r1, R2 && r2, O out, Cmp cmp, Proj1 proj1, Proj2 proj2)
    {
        auto f1 = std::ranges::begin(r1);
        auto const l1 = std::ranges::end(r1);
        auto f2 = std::ranges::begin(r2);
        auto const l2 = std::ranges::end(r2);
        while (f1 != l1 && f2 != l2) {
            auto const & a = proj1(*f1);
            auto const & b = proj2(*f2);
            if (cmp(a, b)) {
                ++f1;
                continue;
            }
            if (!cmp(b, a))
                *out++ = *f1++;
            ++f2;
        }
        return out;
    }

    void serialize_for_client(
        game_state_t const & gs,
        std::vector<fleet_t const *> const & visible_fleets,
        int nation_id,
        planet_t const & x,
        visibility_kind vis,
        int planet_id,
        ostream_tarray_facade * os)
    {
        if (vis == visibility_kind::owner) {
            serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 0, os);
        } else if (vis == visibility_kind::neutral_or_enemy) {
            auto const & nation = gs.nations[nation_id];

            planet_t copy = x;

            copy.settlement_ids.resize(
                nation.settlements.size() + nation.settlements_seen.size());

            auto const ids_last = detail::set_intersection(
                x.settlement_ids,
                nation.settlements,
                copy.settlement_ids.begin(),
                std::ranges::less{},
                std::identity{},
                [](auto const & e) { return e.id; });
            detail::set_intersection(
                x.settlement_ids,
                nation.settlements_seen,
                ids_last,
                std::ranges::less{},
                std::identity{},
                [](auto const & e) { return e.id; });
            copy.settlement_ids.erase(ids_last, copy.settlement_ids.end());

            if (std::ranges::binary_search(nation.planets_surveyed, planet_id)) {
                serialize_message_impl<ser_op::write, ser_field_op::dont_write>(
                    copy, 0, os);
            } else {
                std::array<int, 5> fields_to_elide = {
                    {metadata<planet_t>::water().index_,
                     metadata<planet_t>::food().index_,
                     metadata<planet_t>::energy().index_,
                     metadata<planet_t>::metal().index_,
                     metadata<planet_t>::fuel_minerals().index_}};
                serialize_message_impl<ser_op::write, ser_field_op::dont_write>(
                    copy, 0, os, fields_to_elide);
            }
        } else {
            detail::serialize_message_end<ser_op::write>(os);
        }
    }

    void serialize_for_client(
        game_state_t const & gs,
        std::vector<fleet_t const *> const & visible_fleets,
        int nation_id,
        nation_t const & x,
        visibility_kind vis,
        int,
        ostream_tarray_facade * os)
    {
#if INSTRUMENT_DE_SERIALIZE_FOR_CLIENT
        switch (vis) {
        case visibility_kind::owner:
            std::cout << std::format("serialize: writing as OWNER\n");
            break;
        case visibility_kind::neutral_or_enemy:
            std::cout << std::format("serialize: writing as NEUTRAL\n");
            break;
        default:
            std::cout << std::format("serialize: writing as UNSEEN\n");
            break;
        }
#endif
        if (vis == visibility_kind::owner) {
            serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 0, os);
        } else if (vis == visibility_kind::neutral_or_enemy) {
            detail::serialize_impl<ser_op::write, ser_field_op::write>(
                x.id, metadata<nation_t>::id().index_, os);

            // field number, size, then elements
            detail::serialize_impl<ser_op::write, ser_field_op::write>(
                x.unit_designs.size(),
                metadata<nation_t>::unit_designs().index_,
                os);
            for (auto const & e : x.unit_designs) {
                auto it = std::ranges::find(
                    gs.nations[nation_id].foreign_designs_seen, e.id);
                if (it != gs.nations[nation_id].foreign_designs_seen.end()) {
                    serialize_for_client(
                        gs,
                        visible_fleets,
                        nation_id,
                        e,
                        visibility_kind::owner,
                        os);
                    continue;
                }

                it = std::ranges::find(
                    gs.nations[nation_id].foreign_designs_glimpsed, e.id);
                if (it !=
                    gs.nations[nation_id].foreign_designs_glimpsed.end()) {
                    serialize_for_client(
                        gs,
                        visible_fleets,
                        nation_id,
                        e,
                        visibility_kind::neutral_or_enemy,
                        os);
                    continue;
                }

                unit_design_t design{invalid_nation_and_object};
                serialize_for_client(
                    gs,
                    visible_fleets,
                    nation_id,
                    design,
                    visibility_kind::neutral_or_enemy,
                    os);
            }

            {
                // field number, size, then elements
                boost::container::small_vector<visibility_kind, 1024> fleet_vis(
                    x.fleets.size(), visibility_kind::unseen);
                for (auto * f : visible_fleets) {
                    if (f->id.nation_id == x.id) {
                        fleet_vis[f->id.object_id] =
                            visibility_kind::neutral_or_enemy;
                    }
                }
                uint32_t const num_visible_fleets =
                    x.fleets.size() -
                    std::ranges::count(fleet_vis, visibility_kind::unseen);
                detail::serialize_impl<ser_op::write, ser_field_op::write>(
                    num_visible_fleets,
                    metadata<nation_t>::fleets().index_,
                    os);
                for (int i = 0, last = (int)x.fleets.size(); i < last; ++i) {
                    if (fleet_vis[i] == visibility_kind::unseen)
                        continue;
                    serialize_for_client(
                        gs,
                        visible_fleets,
                        nation_id,
                        x.fleets[i],
                        fleet_vis[i],
                        os);
                }
            }

            detail::serialize_impl<ser_op::write, ser_field_op::write>(
                x.defeated, metadata<nation_t>::defeated().index_, os);

            detail::serialize_message_end<ser_op::write>(os);
        } else {
            detail::serialize_message_end<ser_op::write>(os);
        }
    }

    void deserialize_for_client(
        int & map_width,
        int & map_height,
        std::vector<indexed_object<hex_t>> & hexes,
        std::vector<indexed_object<system_t>> & systems,
        std::vector<indexed_object<planet_t>> & planets,
        std::vector<indexed_object<nation_t>> & nations,
        int & play_speed,
        date_t & date,
        std::span<std::byte const> src)
    {
        uint32_t field_number = -1;
        uint32_t expected_field_number =
            metadata<game_state_t>::map_width().index_;
        src = detail::read_varint(field_number, src);
        if (field_number != expected_field_number) {
            throw failed_deserialization(std::format(
                "deserialize_for_client(): Expected field #{} while reading "
                "message; got {} instead.",
                expected_field_number,
                field_number));
        }
        src = detail::deserialize_impl(map_width, src);
        PRINT_READ_OF(map_width);

        field_number = -1;
        expected_field_number = metadata<game_state_t>::map_height().index_;
        src = detail::read_varint(field_number, src);
        if (field_number != expected_field_number) {
            throw failed_deserialization(std::format(
                "deserialize_for_client(): Expected field #{} while reading "
                "message; got {} instead.",
                expected_field_number,
                field_number));
        }
        src = detail::deserialize_impl(map_height, src);
        PRINT_READ_OF(map_height);

        field_number = -1;
        expected_field_number = metadata<game_state_t>::hexes().index_;
        src = detail::read_varint(field_number, src);
#if INSTRUMENT_DE_SERIALIZE_FOR_CLIENT
        std::cout << std::format(
            "DEserialize: read field index {}\n", field_number);
#endif
        if (field_number != expected_field_number) {
            throw failed_deserialization(std::format(
                "deserialize_for_client(): Expected field #{} while reading "
                "message; got {} instead.",
                expected_field_number,
                field_number));
        }
        src = detail::deserialize_for_client(hexes, src);

        field_number = -1;
        expected_field_number = metadata<game_state_t>::systems().index_;
        src = detail::read_varint(field_number, src);
#if INSTRUMENT_DE_SERIALIZE_FOR_CLIENT
        std::cout << std::format(
            "DEserialize: read field index {}\n", field_number);
#endif
        if (field_number != expected_field_number) {
            throw failed_deserialization(std::format(
                "deserialize_for_client(): Expected field #{} while reading "
                "message; got {} instead.",
                expected_field_number,
                field_number));
        }
        src = detail::deserialize_for_client(systems, src);

        field_number = -1;
        expected_field_number = metadata<game_state_t>::planets().index_;
        src = detail::read_varint(field_number, src);
#if INSTRUMENT_DE_SERIALIZE_FOR_CLIENT
        std::cout << std::format(
            "DEserialize: read field index {}\n", field_number);
#endif
        if (field_number != expected_field_number) {
            throw failed_deserialization(std::format(
                "deserialize_for_client(): Expected field #{} while reading "
                "message; got {} instead.",
                expected_field_number,
                field_number));
        }
        src = detail::deserialize_for_client(planets, src);

        field_number = -1;
        expected_field_number = metadata<game_state_t>::nations().index_;
        src = detail::read_varint(field_number, src);
#if INSTRUMENT_DE_SERIALIZE_FOR_CLIENT
        std::cout << std::format(
            "DEserialize: read field index {}\n", field_number);
#endif
        if (field_number != expected_field_number) {
            throw failed_deserialization(std::format(
                "deserialize_for_client(): Expected field #{} while reading "
                "message; got {} instead.",
                expected_field_number,
                field_number));
        }
        src = detail::deserialize_for_client(nations, src);

        // TODO: Alliances too.

        field_number = -1;
        expected_field_number = metadata<game_state_t>::play_speed().index_;
        src = detail::read_varint(field_number, src);
        if (field_number != expected_field_number) {
            throw failed_deserialization(std::format(
                "deserialize_for_client(): Expected field #{} while reading "
                "message; got {} instead.",
                expected_field_number,
                field_number));
        }
        src = detail::deserialize_impl(play_speed, src);
        PRINT_READ_OF(play_speed);

        field_number = -1;
        expected_field_number = metadata<game_state_t>::date().index_;
        src = detail::read_varint(field_number, src);
        if (field_number != expected_field_number) {
            throw failed_deserialization(std::format(
                "deserialize_for_client(): Expected field #{} while reading "
                "message; got {} instead.",
                expected_field_number,
                field_number));
        }
        src = detail::deserialize_impl(date, src);
        PRINT_READ_OF(date);

        field_number = -1;
        src = detail::read_varint(field_number, src);
        if (field_number != 0) {
            throw failed_deserialization(std::format(
                "deserialize_for_client(): Expected terminal zero at end of "
                "message; got {} instead.",
                field_number));
        }
    }
}
