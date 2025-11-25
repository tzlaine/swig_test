#pragma once

#include "game_data.hpp"
#include "map_util.hpp"


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

    proximity_grid(box_2d grid_extents, double r)
    {
        bias_ = grid_extents.min_;
        width_ = (int)std::ceil(extent(grid_extents).x / cell_size);
        height_ = (int)std::ceil(extent(grid_extents).y / cell_size);
        cells_.resize(width_ * height_);
        max_detection_radius(r);
    }

    void clear_pointers()
    {
        pointers_.clear();
        erased_pointers_.clear();
    }

    void insert(T const & x)
    {
        pointers_.push_back(&x);
        dirty_ = true;
    }
    void erase(T const & x)
    {
        erased_pointers_.push_back(&x);
        dirty_ = true;
    }

    template<typename R>
    void gather_visible_objects(
        std::vector<pointer> & retval,
        int owner_id,
        R && owner_ptrs,
        game_state_t const & gs,
        find_visible find)
    {
        reconcile();
        for (auto ptr : owner_ptrs) {
            auto * cell = cell_for(cell_pos(position(ptr)));
            if (!cell)
                continue;
            for (auto cell_ptr : cell->pointers_) {
                if (find == find_visible::non_allied &&
                    (owner_id == owner(cell_ptr) ||
                     allied(gs, owner_id, owner(cell_ptr)))) {
                    continue;
                }
                if (dist_sq(position(ptr), position(cell_ptr)) <
                    detection_dist_sq(gs, ptr, cell_ptr)) {
                    retval.push_back(cell_ptr);
                }
            }
        }
    }

    double max_detection_radius() const { return max_detection_radius_; }
    void max_detection_radius(double r)
    {
        if (std::abs(r - max_detection_radius_) < epsilon)
            return;

        max_detection_radius_ = r;

        int const max_cell_radius = int(max_detection_radius_ / cell_size) + 1;

        double const max_radius_sq =
            max_detection_radius_ * max_detection_radius_;

        box_2d const center_cell_box{
            point_2d{-cell_size / 2.0, -cell_size / 2.0},
            point_2d{cell_size / 2.0, cell_size / 2.0}};

        max_radius_offsets_.clear();
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
                    max_radius_offsets_.push_back(int_point{i, j});
            }
        }

        // sort to optimize for cache locality
        std::ranges::sort(
            max_radius_offsets_, [](auto const & a, auto const & b) {
                if (a.y < b.y)
                    return true;
                if (b.y < a.y)
                    return true;
                return a.x < b.x;
            });
    }

private:
    struct cell
    {
        std::vector<pointer> pointers_;
    };

    std::vector<pointer> pointers_;
    std::vector<pointer> erased_pointers_;

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

    void insert(pointer ptr)
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
            insert(p);
        }

        dirty_ = false;
    }

    std::vector<int_point> max_radius_offsets_;

    std::vector<cell> cells_;
    int height_ = 1;
    int width_ = 1;
    point_2d bias_;
    double max_detection_radius_ = 0.0;

    bool dirty_ = true;

    inline static double const cell_size = CellSize;
};
