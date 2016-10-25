#include "root_widget.h"

#include <adobe/future/modal_dialog_interface.hpp>
#include <adobe/future/widgets/headers/platform_window.hpp>

#include <boost/polymorphic_cast.hpp>


/** Contains the result of a modal dialog created by modal_dialog(). */
struct modal_dialog_result_t
{
    /** A map from adobe::name_t to value (adobe::any_regular_t) of the
        expected results.  The contents of m_results will depend on the value
        specified for "result" in the Adam script associated with the modal
        dialog. */
    adobe::dictionary_t result_;

    /** Indicates the name of the action that terminated the dialog (e.g. "ok"
        or "cancel"). */
    adobe::name_t terminating_action_;
};

#if 0 // TODO
modal_dialog_result_t modal_dialog (
    UWorld * world,
    std::istream & eve_definition,
    std::string const & eve_filename,
    std::istream & adam_definition,
    std::string const & adam_filename
) {
    modal_dialog_result_t retval;

    adobe::window_t::create_widget_world_ = world;

    std::unique_ptr<adobe::modal_dialog_t> eve_modal_dialog(new adobe::modal_dialog_t);
    eve_modal_dialog->input_m = adobe::dictionary_t();
    eve_modal_dialog->record_m = adobe::dictionary_t();
    eve_modal_dialog->display_state_m = adobe::dictionary_t();
    eve_modal_dialog->display_options_m = adobe::dialog_display_s;
    eve_modal_dialog->working_directory_m = boost::filesystem::path();
    eve_modal_dialog->parent_m = nullptr;

    UWidget * widget = eve_modal_dialog->init(eve_definition, eve_filename, adam_definition, adam_filename);
    Uroot_widget * root_widget = boost::polymorphic_downcast<Uroot_widget *>(widget);
    // TODO: Move to a certain location (center, maybe).
    adobe::dialog_result_t adobe_result = dialog->go(); // TODO: Add modal-pumping code to this function.

    retval.result_ = std::move(adobe_result.command_m);
    retval.terminating_action_ = adobe_result.terminating_action_m;

    return retval;
}
#endif

struct adam_eve_ui_t
{
    adam_eve_ui_t () :
        root_widget_ ()
    {}

    adam_eve_ui_t (
        UWorld * world,
        std::istream & eve_definition,
        std::string const & eve_filename,
        std::istream & adam_definition,
        std::string const & adam_filename
    ) {
        adobe::window_t::create_widget_world_ = world;

        eve_modal_dialog_.reset(new adobe::modal_dialog_t);
        eve_modal_dialog_->input_m = adobe::dictionary_t();
        eve_modal_dialog_->record_m = adobe::dictionary_t();
        eve_modal_dialog_->display_state_m = adobe::dictionary_t();
        eve_modal_dialog_->display_options_m = adobe::dialog_display_s;
        eve_modal_dialog_->working_directory_m = boost::filesystem::path();
        eve_modal_dialog_->parent_m = nullptr;

        UWidget * widget =
            eve_modal_dialog_->init(eve_definition/*, eve_filename*/, adam_definition/*, adam_filename*/);
        root_widget_ = boost::polymorphic_downcast<Uroot_widget *>(widget);
    }

    Uroot_widget & root_widget ()
    { return *root_widget_; }

private:
    std::unique_ptr<adobe::modal_dialog_t> eve_modal_dialog_;
    Uroot_widget * root_widget_;
};
