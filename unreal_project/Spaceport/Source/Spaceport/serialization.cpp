#include "serialization.hpp"

#include "game_data_metadata.hpp"
#include "model.hpp"


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

    void serialize_for_client(
        game_state_t const & gs,
        std::vector<fleet_t const *> const & visible_fleets,
        int nation_id,
        planet_t const & x,
        visibility_kind vis,
        int,
        ostream_tarray_facade * os)
    {
        if (vis == visibility_kind::owner) {
            serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 0, os);
        } else if (vis == visibility_kind::neutral_or_enemy) {
            std::array<int, 8> fields_to_elide = {
                {metadata<planet_t>::water().index_,
                 metadata<planet_t>::food().index_,
                 metadata<planet_t>::energy().index_,
                 metadata<planet_t>::metal().index_,
                 metadata<planet_t>::fuel().index_,
                 metadata<planet_t>::population().index_,
                 metadata<planet_t>::infrastructure().index_,
                 metadata<planet_t>::garrison().index_}};
            auto const nonowner_effect = [](auto const & e) {
                return transitory(e);
            };
            if (std::ranges::any_of(x.effects, nonowner_effect)) {
                planet_t copy = x;
                std::erase_if(copy.effects, nonowner_effect);
                serialize_message_impl<ser_op::write, ser_field_op::dont_write>(
                    copy, 0, os, fields_to_elide);
            } else {
                serialize_message_impl<ser_op::write, ser_field_op::dont_write>(
                    x, 0, os, fields_to_elide);
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
        if (vis == visibility_kind::owner) {
            serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 0, os);
        } else if (vis == visibility_kind::neutral_or_enemy) {
            detail::serialize_impl<ser_op::write, ser_field_op::write>(
                x.id, 1, os);

            // field number, size, then elements
            detail::serialize_impl<ser_op::write, ser_field_op::dont_write>(
                2, 0, os);
            detail::serialize_impl<ser_op::write, ser_field_op::dont_write>(
                x.unit_designs.size(), 0, os);
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
                if (it != gs.nations[nation_id].foreign_designs_seen.end()) {
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

            // field number, size, then elements
            detail::serialize_impl<ser_op::write, ser_field_op::dont_write>(
                4, 0, os);
            detail::serialize_impl<ser_op::write, ser_field_op::dont_write>(
                x.fleets.size(), 0, os);
            for (auto const & e : x.fleets) {
                serialize_for_client(
                    gs,
                    visible_fleets,
                    nation_id,
                    e,
                    visibility_kind::neutral_or_enemy,
                    os);
            }

            detail::serialize_impl<ser_op::write, ser_field_op::write>(
                x.defeated, 12, os);
        } else {
            detail::serialize_message_end<ser_op::write>(os);
        }
    }

    void deserialize_for_client(
        int map_width,
        int map_height,
        std::vector<indexed_object<hex_t>> & hexes,
        std::vector<indexed_object<system_t>> & systems,
        std::vector<indexed_object<planet_t>> & planets,
        std::vector<indexed_object<nation_t>> & nations,
        std::span<std::byte const> src)
    {
        uint32_t field_number = -1;
        src = detail::read_varint(field_number, src);
        if (field_number != 1) {
            throw failed_deserialization(std::format(
                "deserialize_for_client(): Expected field #1 while reading "
                "message; got {} instead.",
                field_number));
        }
        src = detail::deserialize_impl(map_width, src);

        field_number = -1;
        src = detail::read_varint(field_number, src);
        if (field_number != 2) {
            throw failed_deserialization(std::format(
                "deserialize_for_client(): Expected field #2 while reading "
                "message; got {} instead.",
                field_number));
        }
        src = detail::deserialize_impl(map_height, src);

        field_number = -1;
        src = detail::read_varint(field_number, src);
        if (field_number != 3) {
            throw failed_deserialization(std::format(
                "deserialize_for_client(): Expected field #3 while reading "
                "message; got {} instead.",
                field_number));
        }
        src = detail::deserialize_for_client(hexes, src);

        field_number = -1;
        src = detail::read_varint(field_number, src);
        if (field_number != 4) {
            throw failed_deserialization(std::format(
                "deserialize_for_client(): Expected field #4 while reading "
                "message; got {} instead.",
                field_number));
        }
        src = detail::deserialize_for_client(systems, src);

        field_number = -1;
        src = detail::read_varint(field_number, src);
        if (field_number != 5) {
            throw failed_deserialization(std::format(
                "deserialize_for_client(): Expected field #5 while reading "
                "message; got {} instead.",
                field_number));
        }
        src = detail::deserialize_for_client(planets, src);

        field_number = -1;
        src = detail::read_varint(field_number, src);
        if (field_number != 6) {
            throw failed_deserialization(std::format(
                "deserialize_for_client(): Expected field #6 while reading "
                "message; got {} instead.",
                field_number));
        }
        src = detail::deserialize_for_client(nations, src);

        // TODO: Alliances too.

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
