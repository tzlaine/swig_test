#pragma once

#include "check.hpp"
#include "game_data.hpp"
#include "map_util.hpp"
#include "model_util.hpp"


namespace detail {
    inline std::vector<int_point>
    max_radius_offsets(double max_radius, double cell_size)
    {
        check(0.0 < max_radius);
        check(0.0 < cell_size);

        std::vector<int_point> retval;

        int const max_cell_radius = int(max_radius / cell_size) + 1;
        double const max_radius_sq = max_radius * max_radius;

        box_2d const center_cell_box{
            point_2d{-cell_size / 2.0, -cell_size / 2.0},
            point_2d{cell_size / 2.0, cell_size / 2.0}};

        for (int i = -max_cell_radius; i <= max_cell_radius; ++i) {
            for (int j = -max_cell_radius; j <= max_cell_radius; ++j) {
                box_2d const cell_box{
                    point_2d{(i - 0.5) * cell_size, (j - 0.5) * cell_size},
                    point_2d{(i + 0.5) * cell_size, (j + 0.5) * cell_size}};
                double const distances[4] = {
                    closest_distance_sq(cell_box, center_cell_box.min_),
                    closest_distance_sq(cell_box, center_cell_box.max_),
                    closest_distance_sq(
                        cell_box,
                        point_2d{
                            center_cell_box.min_.x, center_cell_box.max_.y}),
                    closest_distance_sq(
                        cell_box,
                        point_2d{
                            center_cell_box.max_.x, center_cell_box.min_.y})};
                double const nearest_sq = std::ranges::min(distances);
                if (nearest_sq < max_radius_sq)
                    retval.push_back(int_point{i, j});
            }
        }

        // sort to optimize for cache locality
        std::ranges::sort(
            retval, [](auto const & a, auto const & b) {
                if (a.y < b.y)
                    return true;
                if (b.y < a.y)
                    return false;
                return a.x < b.x;
            });

        return retval;
    }
}

template<typename T>
void push_back(std::vector<T *> & v, T * p)
{
    v.push_back(p);
}

enum struct find_visible { all, non_allied };

template<typename T, int CellSize = 4>
    requires requires(T * p, game_state_t const & gs) {
        { owner(p) } -> std::same_as<int>;
        { position(p) } -> std::same_as<point_2d>;
        { detection_dist_sq(gs, p, p) } -> std::same_as<double>;
    }
struct proximity_grid
{
    using pointer = T *;

    proximity_grid() = default;

    proximity_grid(game_state_t const & gs)
    {
        box_2d const grid_extents = world_map_extent(gs);
        bias_ = grid_extents.min_;
        width_ = (int)std::ceil(extent(grid_extents).x / cell_size);
        height_ = (int)std::ceil(extent(grid_extents).y / cell_size);
        cells_.resize(width_ * height_);
        double const r = max_detection_radius_before_stealth(gs);
        max_detection_radius(r);
    }

    void clear_pointers()
    {
        pointers_.clear();
        erased_pointers_.clear();
    }

    void insert(T & x)
    {
        pointers_.push_back(&x);
        dirty_ = true;
    }
    void insert(T * p)
    {
        pointers_.push_back(p);
        dirty_ = true;
    }
    void erase(T & x)
    {
        erased_pointers_.push_back(&x);
        dirty_ = true;
    }
    void erase(T * p)
    {
        erased_pointers_.push_back(p);
        dirty_ = true;
    }

    template<typename C, typename R>
        requires requires(C & c) { push_back(c, pointer{}); }
    void gather_visible_objects(
        C & retval,
        int owner_id,
        R && owner_objects_or_pointers,
        game_state_t const & gs,
        find_visible find)
    {
        reconcile();
        for (auto e : owner_objects_or_pointers) {
            auto * ptr = to_ptr(e);
            auto * cell = cell_for(cell_pos(position(ptr)));
            if (!cell)
                continue;
            for (auto other_ptr : cell->pointers_) {
                if (find == find_visible::non_allied &&
                    (owner_id == owner(other_ptr) ||
                     allied(gs, owner_id, owner(other_ptr)))) {
                    continue;
                }
                // TODO: Looks like the autodetection of allied units is
                // missing here.
                if (dist_sq(position(ptr), position(other_ptr)) <
                    detection_dist_sq(gs, ptr, other_ptr)) {
                    push_back(retval, other_ptr);
                }
            }
        }
    }

    void prepare_to_gather_objects() { reconcile(); }

    double max_detection_radius() const { return max_detection_radius_; }
    void max_detection_radius(double r)
    {
        if (std::abs(r - max_detection_radius_) < epsilon)
            return;
        max_detection_radius_ = r;
        max_radius_offsets_ =
            detail::max_radius_offsets(max_detection_radius_, cell_size);
    }

private:
    struct cell
    {
        std::vector<pointer> pointers_;
    };

    int_point cell_pos(point_2d pos) const
    {
        pos = pos - bias_;
        int const x = int(pos.x / cell_size);
        int const y = int(pos.y / cell_size);
        return {x, y};
    }

    cell const * cell_for(int_point pos) const
    {
        auto const i = pos.x + pos.y * width_;
        if (i < 0 || (int)cells_.size() <= i)
            return nullptr;
        return &cells_[i];
    }
    cell * cell_for(int_point pos) {
        auto const i = pos.x + pos.y * width_;
        if (i < 0 || (int)cells_.size() <= i)
            return nullptr;
        return &cells_[i];
    }

    void place_in_cells(pointer ptr)
    {
        auto const cell_pos_ = cell_pos(position(ptr));
        for (auto offset : max_radius_offsets_) {
            auto const pos = cell_pos_ + offset;
            if (auto * cell = cell_for(pos))
                cell->pointers_.push_back(ptr);
        }
    }

    void reconcile()
    {
        if (!dirty_)
            return;

        std::ranges::fill(cells_, cell{});

        std::ranges::sort(erased_pointers_, std::ranges::less{});
        std::erase_if(pointers_, [&](auto e) {
            return std::ranges::binary_search(erased_pointers_, e);
        });
        erased_pointers_.clear();

        for (auto p : pointers_) {
            place_in_cells(p);
        }

        dirty_ = false;
    }

    template<typename T>
    T * to_ptr(T * p)
    {
        return p;
    }
    template<typename T>
    T * to_ptr(T & p)
    {
        return &p;
    }

    std::vector<pointer> pointers_;
    std::vector<pointer> erased_pointers_;

    std::vector<int_point> max_radius_offsets_;

    std::vector<cell> cells_;
    int height_ = 1;
    int width_ = 1;
    point_2d bias_;
    double max_detection_radius_ = 0.0;

    bool dirty_ = true;

    inline static double const cell_size = CellSize;
};
