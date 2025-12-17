#include <generate_galaxy.hpp>
#include <model.hpp>
#include <serialization.hpp>
#include <task_system.hpp>

#include <gtest/gtest.h>

#include <format>


template<std::ranges::range R>
auto byte_span_of(R const & r)
{
    return std::as_bytes(std::span(r.begin(), r.end()));
}

unit_design_t design(int nation_id, int id)
{
    int const factor = id + 1;
    return {
        .id = nation_and_object_id_t{nation_id, id},
        .hull = factor,
        .armor = factor,
        .propulsion = 3 / factor,
        .weapons = factor,
        .shields = factor,
        .detection = 3 / factor,
        .stealth = 3 / factor,
        .automation = factor,
        .attack = factor,
        .defense = factor,
        .ground_attack = factor};
}

unit_design_t design(int nation_id, int id, game_state_t const & gs)
{
    return gs.nations[nation_id].unit_designs[id];
}

fleet_t fleet(int nation_id)
{
    return fleet_t{
        .id = nation_and_object_id_t{nation_id, 0},
        .mission = mission_t::explore,
        .units =
            {{.id = {nation_id, 0}, .health = 100},
             {.id = {nation_id, 1}, .health = 100},
             {.id = {nation_id, 2}, .health = 100}},
        .fuel = 100.0f,
        .rounds = 100,
        .missiles = 100,
        .fighters = 25,
        .position = {
            .world_pos_x = 1000.0 * nation_id,
            .world_pos_y = 1000.0 * nation_id,
            .system_id = 10 + nation_id,
            .at_permanent_location = true,
            .location_index = 2,
            .object_index = 3,
            .is_garrison = true}};
}

nation_t nation(int nation_id, game_state_t const & gs)
{
    hex_coord_t const home_hc = {.x = 5, .y = 1 + nation_id};
    int const home_hex = to_index(home_hc, gs.map_width);
    int const home_system = gs.hexes[home_hex].first_system;
    int const home_planet = gs.systems[home_system].first_planet;
    nation_t retval = {
        .id = nation_id,
        .unit_designs =
            {design(nation_id, 0), design(nation_id, 0), design(nation_id, 0)},
        .provinces = {},
        .fleets = {fleet(nation_id)},
        .hexes_seen =
            {home_hex,
             to_index(hex_above_right(home_hc), gs.map_width),
             to_index(hex_above(home_hc), gs.map_width),
             to_index(hex_above_left(home_hc), gs.map_width),
             to_index(hex_below_left(home_hc), gs.map_width),
             to_index(hex_below(home_hc), gs.map_width),
             to_index(hex_below_right(home_hc), gs.map_width)},
        .systems_present_in = {home_system},
        .systems_visited = {home_system},
        .planets_present_on = {home_planet},
        .planets_surveyed = {home_planet},
        .foreign_designs_seen =
            {{.nation_id = (nation_id + 1) % 3, .object_id = 1}},
        .foreign_designs_glimpsed = {},
        .defeated = false};
    return retval;
}

auto visible_fleets(int nation_id, game_state_t const & gs)
{
    return std::vector<fleet_t const *>(
        {&gs.nations[(nation_id + 1) % 3].fleets[0],
         &gs.nations[(nation_id + 2) % 3].fleets[0]});
}

game_state_t const gs = [] {
    task_system ts(4);
    game_state_t retval;
    generation::generate_galaxy(default_game_start_params(), retval, &ts);
    retval.nations = {nation(0, retval), nation(1, retval), nation(2, retval)};
    return retval;
}();

TEST(client_serialization_tests, serialize_for_client_single_object)
{
    using namespace detail;

    // unit_design_t
    {
        std::vector<char> serialized;
        ostream_tarray_facade oss(serialized);
        int const nation_id = 0;
        std::vector<fleet_t const *> visible_fleets;
        unit_design_t const design = ::design(nation_id, 0);

        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                design,
                visibility_kind::owner,
                &oss);

            unit_design_t client_design = {};
            auto const bytes =
                deserialize_impl(client_design, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_EQ(client_design, design);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                design,
                visibility_kind::neutral_or_enemy,
                &oss);

            unit_design_t client_design = {};
            auto const bytes =
                deserialize_impl(client_design, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_NE(client_design, design);

            EXPECT_EQ(client_design.id, design.id);
            EXPECT_EQ(client_design.hull, design.hull);

            unit_design_t const default_design;
            EXPECT_EQ(client_design.armor, default_design.armor);
            EXPECT_EQ(client_design.propulsion, default_design.propulsion);
            EXPECT_EQ(client_design.weapons, default_design.weapons);
            EXPECT_EQ(client_design.shields, default_design.shields);
            EXPECT_EQ(client_design.detection, default_design.detection);
            EXPECT_EQ(client_design.stealth, default_design.stealth);
            EXPECT_EQ(client_design.automation, default_design.automation);
            EXPECT_EQ(client_design.attack, default_design.attack);
            EXPECT_EQ(client_design.defense, default_design.defense);
            EXPECT_EQ(
                client_design.ground_attack, default_design.ground_attack);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                design,
                visibility_kind::unseen,
                &oss);
            EXPECT_EQ(byte_span_of(serialized).size(), 1u);
            EXPECT_EQ(byte_span_of(serialized)[0], std::byte(0));
        }
    }

    // unit_t
    {
        std::vector<char> serialized;
        ostream_tarray_facade oss(serialized);
        int const nation_id = 0;
        std::vector<fleet_t const *> visible_fleets;
        unit_t const unit = {.id = {3, 4}, .health = 100};

        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                unit,
                visibility_kind::owner,
                &oss);

            unit_t client_unit = {};
            auto const bytes =
                deserialize_impl(client_unit, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_EQ(client_unit, unit);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                unit,
                visibility_kind::neutral_or_enemy,
                &oss);

            unit_t client_unit = {};
            auto const bytes =
                deserialize_impl(client_unit, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_NE(client_unit, unit);

            EXPECT_EQ(client_unit.id, unit.id);

            unit_t const default_unit;
            EXPECT_EQ(client_unit.health, default_unit.health);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                unit,
                visibility_kind::unseen,
                &oss);
            EXPECT_EQ(byte_span_of(serialized).size(), 1u);
            EXPECT_EQ(byte_span_of(serialized)[0], std::byte(0));
        }
    }

    // fleet_t
    {
        std::vector<char> serialized;
        ostream_tarray_facade oss(serialized);
        int const nation_id = 0;
        std::vector<fleet_t const *> visible_fleets;
        fleet_t const fleet = ::fleet(5);

        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                fleet,
                visibility_kind::owner,
                &oss);

            fleet_t client_fleet = {};
            auto const bytes =
                deserialize_impl(client_fleet, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_EQ(client_fleet, fleet);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                fleet,
                visibility_kind::neutral_or_enemy,
                &oss);

            fleet_t client_fleet = {};
            auto const bytes =
                deserialize_impl(client_fleet, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_NE(client_fleet, fleet);

            EXPECT_EQ(client_fleet.id, fleet.id);

            EXPECT_EQ(client_fleet.units.size(), fleet.units.size());
            EXPECT_EQ(client_fleet.units[0].id, fleet.units[0].id);
            EXPECT_EQ(client_fleet.units[0].health, -1);
            EXPECT_EQ(client_fleet.units[1].id, fleet.units[1].id);
            EXPECT_EQ(client_fleet.units[1].health, -1);
            EXPECT_EQ(client_fleet.units[2].id, fleet.units[2].id);
            EXPECT_EQ(client_fleet.units[2].health, -1);

            EXPECT_EQ(client_fleet.position, fleet.position);

            fleet_t const default_fleet;
            EXPECT_EQ(client_fleet.mission, default_fleet.mission);
            EXPECT_EQ(client_fleet.fuel, default_fleet.fuel);
            EXPECT_EQ(client_fleet.rounds, default_fleet.rounds);
            EXPECT_EQ(client_fleet.missiles, default_fleet.missiles);
            EXPECT_EQ(client_fleet.fighters, default_fleet.fighters);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                fleet,
                visibility_kind::unseen,
                &oss);
            EXPECT_EQ(byte_span_of(serialized).size(), 1u);
            EXPECT_EQ(byte_span_of(serialized)[0], std::byte(0));
        }
    }

    // hex_t
    {
        std::vector<char> serialized;
        ostream_tarray_facade oss(serialized);
        int const nation_id = 0;
        std::vector<fleet_t const *> visible_fleets;
        hex_t const hex = gs.hexes.front();

        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                hex,
                visibility_kind::owner,
                0,
                &oss);

            hex_t client_hex = {};
            auto const bytes =
                deserialize_impl(client_hex, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_EQ(client_hex, hex);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                hex,
                visibility_kind::neutral_or_enemy,
                0,
                &oss);

            hex_t client_hex = {};
            auto const bytes =
                deserialize_impl(client_hex, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_EQ(client_hex, hex);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                hex,
                visibility_kind::unseen,
                0,
                &oss);
            EXPECT_EQ(byte_span_of(serialized).size(), 1u);
            EXPECT_EQ(byte_span_of(serialized)[0], std::byte(0));
        }
    }

    // system_t
    {
        std::vector<char> serialized;
        ostream_tarray_facade oss(serialized);
        int const nation_id = 0;
        std::vector<fleet_t const *> visible_fleets;
        system_t const system = gs.systems.front();

        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                system,
                visibility_kind::owner,
                0,
                &oss);

            system_t client_system = {};
            auto const bytes =
                deserialize_impl(client_system, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_EQ(client_system, system);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                system,
                visibility_kind::neutral_or_enemy,
                0,
                &oss);

            system_t client_system = {};
            auto const bytes =
                deserialize_impl(client_system, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_EQ(client_system, system);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                system,
                visibility_kind::unseen,
                0,
                &oss);
            EXPECT_EQ(byte_span_of(serialized).size(), 1u);
            EXPECT_EQ(byte_span_of(serialized)[0], std::byte(0));
        }
    }
}

struct fleet_t2
{
    nation_and_object_id_t id = {};
    mission_t mission = mission_t::invalid_mission;
    std::vector<unit_t> units = {};
    float fuel = -1.0f;
    int rounds = -1;
    int missiles = -1;
    int fighters = -1;
    fleet_position_t position = {};
    bool operator==(fleet_t2 const &) const = default;
};

#include <boost/callable_traits.hpp>
#include <boost/type_index/ctti_type_index.hpp>

struct bad_any_ref_cast : std::exception
{
    bad_any_ref_cast(std::string msg) : msg_(msg) {}

    char const * what() const override { return msg_.c_str(); }

private:
    std::string msg_;
};

enum struct numeric_type : char {
    not_numeric = 0,
    bool_,
    int_,
    unsigned_int_,
    size_t_,
    float_,
    double_
};

namespace detail {
    template<typename T>
    numeric_type numeric_type_of(T const &)
    {
        return numeric_type::not_numeric;
    }
    numeric_type numeric_type_of(bool const &) { return numeric_type::bool_; }
    numeric_type numeric_type_of(int const &) { return numeric_type::int_; }
    numeric_type numeric_type_of(unsigned int const &)
    {
        return numeric_type::unsigned_int_;
    }
    numeric_type numeric_type_of(size_t const &)
    {
        return numeric_type::size_t_;
    }
    numeric_type numeric_type_of(float const &) { return numeric_type::float_; }
    numeric_type numeric_type_of(double const &)
    {
        return numeric_type::double_;
    }

    struct any_ref_members
    {
        void * ptr_ = nullptr;
        boost::typeindex::ctti_type_index ctti_;
        bool const_ = false;
        numeric_type numeric_type_ = numeric_type::not_numeric;
    };

    struct expr_op_members
    {
        void * impl_ = nullptr;
    };
}

struct any_ref
{
    any_ref() = default;

    template<typename T>
    any_ref(T & x) :
        members_{
            .ptr_ = const_cast<std::remove_const_t<T> *>(std::addressof(x)),
            .ctti_ = boost::typeindex::ctti_type_index::type_id<T>(),
            .const_ = std::is_const_v<T>,
            .numeric_type_ = detail::numeric_type_of(x)}
    {}

    bool empty() const { return !members_.ptr_; }
    bool numeric() const
    {
        return members_.numeric_type_ != numeric_type::not_numeric;
    }
    numeric_type get_numeric_type() const { return members_.numeric_type_; }

    template<typename T>
    friend T & cast(any_ref const & a)
    {
        if (!a.members_.ptr_)
            throw bad_any_ref_cast("Attempted to cast an empty any_ref.");
        if (boost::typeindex::ctti_type_index::type_id<T>() !=
            a.members_.ctti_) {
            throw bad_any_ref_cast(
                "Attempted to cast an any_ref to the wrong type.");
        }
        if constexpr (std::is_const_v<T>) {
            return *static_cast<T *>(a.members_.ptr_);
        } else if (a.members_.const_) {
            throw bad_any_ref_cast(
                "Attempted to cast a const-typed any_ref to a non-const type.");
        } else {
            return *static_cast<T *>(a.members_.ptr_);
        }
    }

private:
    detail::any_ref_members members_;
};

enum struct operator_kind {
    invalid_operator_kind = 0,
    assign,
    plus,
    minus,
    times,
    divide,
    unary_plus,
    unary_minus,
    call
    // TODO: etc.
};

struct call
{
    int function_;
};

struct operation
{
    operator_kind op_;
};

struct expr_op;

using expr_op_stack = std::vector<expr_op>;

namespace detail {
    template<typename F>
    auto args_tuple()
    {
        if constexpr (requires { typename boost::callable_traits::args_t<F>; })
            return boost::callable_traits::args_t<F>{};
        else
            return std::tuple<>{};
    }
    template<typename T>
    constexpr auto arity_of()
    {
        if constexpr (requires { typename boost::callable_traits::args_t<T>; })
            return std::tuple_size<decltype(args_tuple<T>())>{};
        else
            return -1;
    }

    template<typename Tuple, size_t... Is>
    void fill_tuple(
        Tuple & tup,
        expr_op_stack::iterator first,
        expr_op_stack::iterator last,
        std::integer_sequence<size_t, Is...>)
    {
        ((std::get<Is>(tup) =
              cast<std::tuple_element_t<Is, Tuple>>(*(last - 1 - Is))),
         ...);
    }
}

// TODO: Merge with any_ref.
struct expr_op
{
    using iterator = expr_op_stack::iterator;

    expr_op() = default;
    expr_op(expr_op && other) { std::swap(members_.impl_, other.members_.impl_); }
    expr_op & operator=(expr_op && other)
    {
        delete get_impl();
        members_.impl_ = nullptr;
        std::swap(members_.impl_, other.members_.impl_);
        return *this;
    }
    ~expr_op() { delete get_impl(); }

    template<typename T>
        requires(!std::same_as<std::remove_cvref_t<T>, expr_op>)
    expr_op(T && x) :
        members_{
            .impl_ =
                static_cast<void *>(new impl<std::remove_cvref_t<T>>((T &&)x))}
    {}

    bool empty() const { return !members_.impl_; }
    bool numeric() const
    {
        return get_numeric_type() != numeric_type::not_numeric;
    }
    int arity() const { return get_impl()->arity(); }
    numeric_type get_numeric_type() const { return get_impl()->get_numeric_type(); }

    expr_op operator()(iterator first, iterator last) const
    {
        return get_impl()->apply(first, last);
    }

    template<typename T>
    friend T & cast(expr_op const & o)
    {
        if (!o.members_.impl_)
            throw bad_any_ref_cast("Attempted to cast an empty expr_op.");
        if (boost::typeindex::ctti_type_index::type_id<T>() != o.ctti()) {
            throw bad_any_ref_cast(
                "Attempted to cast an expr_op to the wrong type.");
        }
        return *static_cast<T *>(o.get_impl()->value());
    }

private:
    struct impl_base
    {
        virtual ~impl_base() {}
        virtual int arity() const = 0;
        virtual numeric_type get_numeric_type() const = 0;
        virtual expr_op apply(iterator first, iterator last) const = 0;
        virtual boost::typeindex::ctti_type_index ctti() const = 0;
        virtual void * value() const = 0;
    };

    template<typename T>
    struct impl : impl_base
    {
        impl(T && x) : op_(std::move(x)) {}

        impl(T const & x) : op_(x) {}

        int arity() const override { return arity_; }

        numeric_type get_numeric_type() const override
        {
            return detail::numeric_type_of(op_);
        }

        expr_op apply(iterator first, iterator last) const override
        {
            if constexpr (requires {
                              typename boost::callable_traits::args_t<T>;
                          }) {
                static_assert(std::same_as<
                              boost::callable_traits::return_type_t<T>,
                              expr_op>);
                assert(last - first == arity());
                auto tup = detail::args_tuple<T>();
                auto const indices = std::make_index_sequence<arity_>{};
                detail::fill_tuple(tup, first, last, indices);
                return std::apply(op_, tup);
            } else {
                return expr_op(std::move(op_));
            }
        }

        boost::typeindex::ctti_type_index ctti() const override
        {
            return boost::typeindex::ctti_type_index::type_id<T>();
        }

        void * value() const override
        {
            return static_cast<void *>(const_cast<T *>(std::addressof(op_)));
        }

        static constexpr int arity_ = detail::arity_of<T>();

        T op_;
    };

    boost::typeindex::ctti_type_index ctti() const
    {
        return get_impl()->ctti();
    }
    impl_base const * get_impl() const
    {
        return static_cast<impl_base const *>(members_.impl_);
    }
    impl_base * get_impl() { return static_cast<impl_base *>(members_.impl_); }

    detail::expr_op_members members_;
};

int arity(expr_op x) { return 0; } // TODO

expr_op evaluate(std::vector<expr_op> & stack)
{
    while (!stack.empty()) {
        expr_op op = std::move(stack.back());
        stack.pop_back();
        int const arity = op.arity();
        if (arity < 0) { // not callable
            assert(stack.empty());
            return op;
        } else {
            assert(arity < (int)stack.size());
            expr_op result = op(stack.end() - arity, stack.end());
            stack.resize(stack.size() - arity);
            stack.emplace_back(std::move(result));
        }
    }
}

constexpr int num_members = -1000;

struct name_and_index
{
    std::string_view name_;
    int index_;
};

template<typename Message, typename T>
struct metadatum : name_and_index
{
    T Message::* ptr_;
};

template<typename T>
struct metadata
{};

template<>
struct metadata<fleet_t2>
{
    static constexpr std::string_view name()
    {
        using namespace std::literals;
        return "fleet_t2";
    }
    static constexpr int lo_field_number() { return 1; }
    static constexpr int hi_field_number() { return 8; }

    static constexpr metadatum<fleet_t2, nation_and_object_id_t> id()
    {
        using namespace std::literals;
        return {"id"sv, 1, &fleet_t2::id};
    }
    static constexpr metadatum<fleet_t2, mission_t> mission()
    {
        using namespace std::literals;
        return {"mission"sv, 2, &fleet_t2::mission};
    }
    static constexpr metadatum<fleet_t2, std::vector<unit_t>> units()
    {
        using namespace std::literals;
        return {"units"sv, 34, &fleet_t2::units};
    }
    static constexpr metadatum<fleet_t2, float> fuel()
    {
        using namespace std::literals;
        return {"fuel"sv, 4, &fleet_t2::fuel};
    }
    static constexpr metadatum<fleet_t2, int> rounds()
    {
        using namespace std::literals;
        return {"rounds"sv, 5, &fleet_t2::rounds};
    }
    static constexpr metadatum<fleet_t2, int> missiles()
    {
        using namespace std::literals;
        return {"missiles"sv, 6, &fleet_t2::missiles};
    }
    static constexpr metadatum<fleet_t2, int> fighters()
    {
        using namespace std::literals;
        return {"fighters"sv, 7, &fleet_t2::fighters};
    }
    static constexpr metadatum<fleet_t2, fleet_position_t> position()
    {
        using namespace std::literals;
        return {"position"sv, 8, &fleet_t2::position};
    }

    static constexpr std::array<name_and_index, 8 + 1> names_and_indices{
        {name_and_index{"", 0},
         name_and_index{"fighters", 7},
         name_and_index{"fuel", 4},
         name_and_index{"id", 1},
         name_and_index{"missiles", 6},
         name_and_index{"mission", 2},
         name_and_index{"position", 8},
         name_and_index{"rounds", 5},
         name_and_index{"units", 3}}};

    static std::function<any_ref(any_ref const &)>
    read_fn(std::string_view field)
    {
        auto it = std::ranges::lower_bound(
            names_and_indices,
            field,
            std::ranges::less{},
            &name_and_index::name_);
        if (it == names_and_indices.end() || it->name_ != field)
            return {};
        switch (it->index_) {
        case 1:
            return [](any_ref const & fleet) {
                return any_ref(cast<fleet_t2 const>(fleet).id);
            };
        case 2:
            return [](any_ref const & fleet) {
                return any_ref(cast<fleet_t2 const>(fleet).mission);
            };
        case 3:
            return [](any_ref const & fleet) {
                return any_ref(cast<fleet_t2 const>(fleet).units);
            };
        case 4:
            return [](any_ref const & fleet) {
                return any_ref(cast<fleet_t2 const>(fleet).fuel);
            };
        case 5:
            return [](any_ref const & fleet) {
                return any_ref(cast<fleet_t2 const>(fleet).rounds);
            };
        case 6:
            return [](any_ref const & fleet) {
                return any_ref(cast<fleet_t2 const>(fleet).missiles);
            };
        case 7:
            return [](any_ref const & fleet) {
                return any_ref(cast<fleet_t2 const>(fleet).fighters);
            };
        case 8:
            return [](any_ref const & fleet) {
                return any_ref(cast<fleet_t2 const>(fleet).position);
            };
        default: return {};
        }
    }
};



TEST(any_ref_tests, all)
{
    double const cd = 13.0;
    double md = 42.0;

    any_ref empty_ref;
    EXPECT_TRUE(empty_ref.empty());
    try {
        int & lang_ref = cast<int>(empty_ref);
        EXPECT_FALSE("Previous line should have thrown.");
    } catch (bad_any_ref_cast const & e) {
    }

    any_ref cd_ref(cd);
    EXPECT_FALSE(cd_ref.empty());
    try {
        int const & int_lang_ref = cast<int const>(cd_ref);
        EXPECT_FALSE("Previous line should have thrown.");
    } catch (bad_any_ref_cast const & e) {
    }
    try {
        double & cd_mut_lang_ref = cast<double>(cd_ref);
        EXPECT_FALSE("Previous line should have thrown.");
    } catch (bad_any_ref_cast const & e) {
    }
    EXPECT_EQ(cast<double const>(cd_ref), cd);

    any_ref md_ref(md);
    try {
        int const & int_lang_ref = cast<int const>(cd_ref);
        EXPECT_FALSE("Previous line should have thrown.");
    } catch (bad_any_ref_cast const & e) {
    }
    EXPECT_FALSE(md_ref.empty());
    EXPECT_EQ(cast<double>(md_ref), md);
    EXPECT_EQ(cast<double const>(md_ref), md);
}

TEST(foo, bar)
{
    {
        expr_op op(42.0);
    }
    {
        expr_op op([] { return expr_op{}; });
    }
}


TEST(client_serialization_tests, TODO)
{
    constexpr auto name = metadata<fleet_t2>::name();

    constexpr auto lo_field_number = metadata<fleet_t2>::lo_field_number();
    constexpr auto hi_field_number = metadata<fleet_t2>::hi_field_number();

    constexpr auto id_meta = metadata<fleet_t2>::id();
    constexpr auto mission_meta = metadata<fleet_t2>::mission();
    constexpr auto units_meta = metadata<fleet_t2>::units();
    constexpr auto fuel_meta = metadata<fleet_t2>::fuel();
    constexpr auto rounds_meta = metadata<fleet_t2>::rounds();
    constexpr auto missiles_meta = metadata<fleet_t2>::missiles();
    constexpr auto fighters_meta = metadata<fleet_t2>::fighters();
    constexpr auto position_meta = metadata<fleet_t2>::position();

    fleet_t2 f{.id = {7, 8}};

    auto read_fn = metadata<fleet_t2>::read_fn("id");
    any_ref f_as_any = f;
    auto id_as_any = read_fn(f);
    EXPECT_FALSE(id_as_any.empty());
    EXPECT_EQ(
        cast<nation_and_object_id_t const>(id_as_any),
        (nation_and_object_id_t{7, 8}));

    std::string effect =
        "nation_self.fleets[j].rounds = "
        "max(nation_self.rounds_stockpile, max_rounds(nation_self.fleets[j]))";
}
