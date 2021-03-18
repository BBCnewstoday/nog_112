#include <noggit/scripting/script_brush.hpp>
#include <noggit/scripting/scripting_tool.hpp>
#include <noggit/scripting/script_settings.hpp>

#include <string>
#include <map>

namespace noggit {
  namespace scripting {
      script_brush_event::script_brush_event(
        math::vector_3d const& pos
        , float outer_radius
        , float inner_radius
        , float dt)
        : _pos(pos)
        , _outer_radius(outer_radius)
        , _inner_radius(inner_radius)
        , _dt(dt)
        {}

    math::vector_3d script_brush_event::pos()
    {
      return _pos;
    }

    float script_brush_event::outer_radius()
    {
      return _outer_radius;
    }

    float script_brush_event::inner_radius()
    {
      return _inner_radius;
    }

    float script_brush_event::dt()
    {
      return _dt;
    }

    script_brush::script_brush(
      scripting_tool * tool
      , std::string const& name
      , sol::protected_function select_event)
      : _tool(tool)
      , _name(name)
      , _select(lua_function<script_brush*>(select_event))
      {};

    void script_brush::set_name(std::string const& name)
    {
      _name = name;
    }

    std::string script_brush::get_name()
    {
      return _name;
    }

    script_settings * script_brush::settings()
    {
      return _tool->get_settings();
    }

    void script_brush::on_left_click(sol::protected_function evt)
    {
      _left_click = lua_function<script_brush_event const&>(evt);
    }

    void script_brush::on_left_hold(sol::protected_function evt)
    {
      _left_hold = lua_function<script_brush_event const&>(evt);
    }

    void script_brush::on_left_release(sol::protected_function evt)
    {
      _left_release = lua_function<script_brush_event const&>(evt);
    }

    void script_brush::on_right_click(sol::protected_function evt)
    {
      _right_click = lua_function<script_brush_event const&>(evt);
    }

    void script_brush::on_right_hold(sol::protected_function evt)
    {
      _right_hold = lua_function<script_brush_event const&>(evt);
    }

    void script_brush::on_right_release(sol::protected_function evt)
    {
      _right_release = lua_function<script_brush_event const&>(evt);
    }

    void register_script_brush(sol::state * state, scripting_tool * tool)
    {
      state->new_usertype<script_brush_event>("script_brush_event"
        ,"pos",&script_brush_event::pos
        ,"outer_radius",&script_brush_event::outer_radius
        ,"inner_radius",&script_brush_event::inner_radius
        ,"dt",&script_brush_event::dt
        );

      state->new_usertype<script_brush>("script_brush"
        ,"set_name",&script_brush::set_name
        ,"settings",&script_brush::settings
        ,"on_left_click",&script_brush::on_left_click
        ,"on_left_hold",&script_brush::on_left_hold
        ,"on_left_release",&script_brush::on_left_release
        ,"on_right_click",&script_brush::on_right_click
        ,"on_right_hold",&script_brush::on_right_hold
        ,"on_right_release",&script_brush::on_right_release
        );
    }
  }
}