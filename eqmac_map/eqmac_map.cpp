#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <cmath>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <boost/math/constants/constants.hpp>

#include <boost/math/special_functions/round.hpp>

#include <boost/range/algorithm.hpp>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

//#include <gl/gl.h>
//#include <gl/glu.h>
//#include "gl/glut.h"
#include "gl/freeglut.h"

#include <windows.h>
#include <psapi.h>

#include "memory.hpp"

#include "eqmac.hpp"

const float PI = boost::math::constants::pi<float>();

std::string application_name = "eqmac_map";

const std::string ini_file = "eqmac_map.ini";

std::stringstream map_zone_ini_file;

memory memory;

double draw_time;
double draw_time_start;
double draw_time_stop;

int draw_time_count = 0;

int draw_fps = 0;

int window_id = 0;

HWND window_hwnd;

int window_width  = 640;
int window_height = 480;

int window_offset_x = -1;
int window_offset_y = -1;

bool window_center = false;

bool window_always_on_top = false;

bool window_start_maximized = false;

int menu_id = 0;

int menu_value = -1;

int menu_num_items = -1;

bool menu_in_use = false;

const int MENU_APP_EXIT                   = 0;
const int MENU_MAP_DRAW_INFO_TEXT_TOGGLE  = 1;
const int MENU_MAP_DRAW_LINES_TOGGLE      = 2;
const int MENU_MAP_DRAW_POINTS_TOGGLE     = 3;
const int MENU_MAP_DRAW_SPAWNS_TOGGLE     = 4;
const int MENU_MAP_DRAW_SPAWN_LIST_TOGGLE = 5;

bool mouse_dragging = false;

int mouse_dragging_start_x = 0;
int mouse_dragging_start_y = 0;

std::string map_folder = "c:/eqmac/maps";

float map_origin_x = window_width  / 2;
float map_origin_y = window_height / 2;

float map_draw_x = 0;
float map_draw_y = 0;

const float MAP_ZOOM_DEFAULT = 1.0;

const float MAP_ZOOM_MIN = 0.001;

float map_zoom = MAP_ZOOM_DEFAULT;

float map_zoom_multiplier = 0.25;

float map_offset_x = 0;
float map_offset_y = 0;

float map_offset_multiplier = 10.0;

int map_max_spawn_names_visible = 100;

float map_max_spawn_distance   = 0.0;
float map_max_spawn_distance_z = 0.0;

float map_max_spawn_distance_multiplier = 8.0;

float map_max_z_positive = 0.0;
float map_max_z_negative = 0.0;

float map_max_z_multiplier = 8.0;

bool map_center_on_target = false;

bool map_draw_info_text = true;

bool map_draw_spawn_list = false;

bool map_draw_fps = false;

bool map_draw_lines     = true;
bool map_draw_points    = true;
bool map_draw_spawns    = false;

bool map_draw_spawn_type_player = true;
bool map_draw_spawn_type_npc    = true;
bool map_draw_spawn_type_corpse = true;

bool map_draw_player_name        = true;
bool map_draw_player_arrow       = true;
bool map_draw_player_corpse_line = true;

bool map_draw_target_line  = true;
bool map_draw_target_arrow = true;

bool map_draw_spawn_name             = true;
bool map_draw_spawn_last_name        = false;
bool map_draw_spawn_level_race_class = true;
bool map_draw_spawn_guild_name       = false;
bool map_draw_spawn_location         = false;
bool map_draw_spawn_heading          = false;
bool map_draw_spawn_distance         = false;
bool map_draw_spawn_distance_z       = false;
bool map_draw_spawn_movement_speed   = false;
bool map_draw_spawn_standing_state   = false;
bool map_draw_spawn_is_holding       = false;

std::string map_zone = "qeynos";

/*
GLUT_BITMAP_9_BY_15
GLUT_BITMAP_8_BY_13
GLUT_BITMAP_HELVETICA_18
GLUT_BITMAP_HELVETICA_12
GLUT_BITMAP_HELVETICA_10
GLUT_BITMAP_TIMES_ROMAN_24
GLUT_BITMAP_TIMES_ROMAN_10
*/
void *font_name = GLUT_BITMAP_HELVETICA_10;

int font_size = 10;

int arrow_by_heading_size = 16;

int num_lines     = 0;
int num_lines_ex  = 0;

int num_points    = 0;
int num_points_ex = 0;

int num_spawns    = 0;
int num_spawns_ex = 0;

struct map_line_t
{
    float from_x;
    float from_y;
    float from_z;

    float to_x;
    float to_y;
    float to_z;

    int r, g, b;
};

std::vector<map_line_t> map_lines;
std::vector<map_line_t>::iterator map_lines_it;

struct map_point_t
{
    float x, y, z;

    int r, g, b;

    int size;

    std::string text;
};

std::vector<map_point_t> map_points;
std::vector<map_point_t>::iterator map_points_it;

struct map_spawn_t
{
    int address;

    int count;

    int spawn_id;
    int owner_id;

    std::string name;
    std::string last_name;

    float x, y, z;

    float distance;
    float distance_z;

    float heading;

    float movement_speed;

    int standing_state;

    int type;
    int level;
    int race;
    int _class; // class is a keyword

    int hp;
    int hp_max;

    bool is_target;
    bool is_player_corpse;

    int is_holding_both;
    int is_holding_secondary;
    int is_holding_primary;
};

std::vector<map_spawn_t> map_spawns;
std::vector<map_spawn_t>::iterator map_spawns_it;

bool spawn_filter_enabled = false;

bool spawn_filter_always_show_players = true;

std::string spawn_filter_name = "";
std::vector<std::string> spawn_filter_name_data;

bool spawn_line_enabled = false;

std::string spawn_line_name = "";
std::vector<std::string> spawn_line_name_data;

std::map<std::string, map_spawn_t> spawn_list;
std::map<std::string, map_spawn_t>::iterator spawn_list_it;
std::map<std::string, map_spawn_t>::iterator spawn_list_it_ex;
std::pair<std::map<std::string, map_spawn_t>::iterator,bool> spawn_list_ret;

float calculate_distance(float x1, float y1, float x2, float y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

template <class T>
T reverse_sign(T value)
{
    T result;

    if (value == 0)
    {
        return value;
    }

    if (value > 0)
    {
        result = value - (value * 2);
    }
    else
    {
        result = abs(value);
    }

    return result;
}

void toggle_bool(bool &b)
{
    b = !b;
}

std::string get_bool_string(bool b)
{
    return b ? "True" : "False";
}

void set_window_always_on_top(HWND hwnd, bool b)
{
    SetWindowPos(hwnd, b ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOSIZE);
}

void center_window()
{
    glutPositionWindow
    (
        (glutGet(GLUT_SCREEN_WIDTH)  - window_width)  / 2,
        (glutGet(GLUT_SCREEN_HEIGHT) - window_height) / 2
    );
}

void parse_ini(std::string ini_filename)
{
    std::fstream file;
    file.open(ini_filename.c_str(), std::ios::in);

    if (!file.is_open())
    {
        return;
    }

    file.close();

    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_filename, pt);

    map_folder = pt.get<std::string>("Options.sMapFolder", map_folder);

    window_width  = pt.get<int>("Window.iWidth",  window_width);
    window_height = pt.get<int>("Window.iHeight", window_height);

    if (window_width > 0 && window_height > 0)
    {
        glutReshapeWindow(window_width, window_height);
    }

    window_offset_x = pt.get<int>("Window.iOffsetX", window_offset_x);
    window_offset_y = pt.get<int>("Window.iOffsetY", window_offset_y);

    if (window_offset_x > -1 && window_offset_y > -1)
    {
        glutPositionWindow(window_offset_x,window_offset_y);
    }

    window_center = pt.get<bool>("Window.bCenter", window_center);

    if (window_center == true)
    {
        center_window();
    }

    window_always_on_top = pt.get<bool>("Window.bAlwaysOnTop", window_always_on_top);

    set_window_always_on_top(window_hwnd, window_always_on_top);

    window_start_maximized = pt.get<bool>("Window.bStartMaximized", window_start_maximized);

    map_zoom            = pt.get<float>("Map.fZoom",           map_zoom);
    map_zoom_multiplier = pt.get<float>("Map.fZoomMultiplier", map_zoom_multiplier);

    map_offset_x          = pt.get<float>("Map.fOffsetX",          map_offset_x);
    map_offset_y          = pt.get<float>("Map.fOffsetY",          map_offset_y);
    map_offset_multiplier = pt.get<float>("Map.fOffsetMultiplier", map_offset_multiplier);

    map_max_spawn_names_visible = pt.get<int>("Map.iMaxSpawnNamesVisible", map_max_spawn_names_visible);

    map_max_spawn_distance            = pt.get<float>("Map.fMaxSpawnDistance",           map_max_spawn_distance);
    map_max_spawn_distance_z          = pt.get<float>("Map.fMaxSpawnDistanceZ",          map_max_spawn_distance_z);
    map_max_spawn_distance_multiplier = pt.get<float>("Map.fMaxSpawnDistanceMultiplier", map_max_spawn_distance_multiplier);

    map_max_z_positive   = pt.get<float>("Map.fMaxZPositive",   map_max_z_positive);
    map_max_z_negative   = pt.get<float>("Map.fMaxZNegative",   map_max_z_negative);
    map_max_z_multiplier = pt.get<float>("Map.fMaxZMultiplier", map_max_z_multiplier);

    map_center_on_target = pt.get<bool>("Map.bCenterOnTarget", map_center_on_target);

    map_draw_info_text = pt.get<bool>("Draw.bInfoText", map_draw_info_text);

    map_draw_spawn_list = pt.get<bool>("Draw.bSpawnList", map_draw_spawn_list);

    map_draw_fps = pt.get<bool>("Draw.bFramesPerSecond", map_draw_fps);

    map_draw_lines  = pt.get<bool>("Draw.bLines",  map_draw_lines);
    map_draw_points = pt.get<bool>("Draw.bPoints", map_draw_points);
    map_draw_spawns = pt.get<bool>("Draw.bSpawns", map_draw_spawns);

    map_draw_spawn_type_player = pt.get<bool>("Draw.bSpawnTypePlayer", map_draw_spawn_type_player);
    map_draw_spawn_type_npc    = pt.get<bool>("Draw.bSpawnTypeNPC",    map_draw_spawn_type_npc);
    map_draw_spawn_type_corpse = pt.get<bool>("Draw.bSpawnTypeCorpse", map_draw_spawn_type_corpse);

    map_draw_player_name         = pt.get<bool>("Draw.bPlayerName",       map_draw_player_name);
    map_draw_player_arrow        = pt.get<bool>("Draw.bPlayerArrow",      map_draw_player_arrow);
    map_draw_player_corpse_line  = pt.get<bool>("Draw.bPlayerCorpseLine", map_draw_player_corpse_line);

    map_draw_target_line  = pt.get<bool>("Draw.bTargetLine",  map_draw_target_line);
    map_draw_target_arrow = pt.get<bool>("Draw.bTargetArrow", map_draw_target_arrow);

    map_draw_spawn_name             = pt.get<bool>("Draw.bSpawnName",           map_draw_spawn_name);
    map_draw_spawn_last_name        = pt.get<bool>("Draw.bSpawnLastName",       map_draw_spawn_last_name);
    map_draw_spawn_level_race_class = pt.get<bool>("Draw.bSpawnLevelRaceClass", map_draw_spawn_level_race_class);
    map_draw_spawn_guild_name       = pt.get<bool>("Draw.bSpawnGuildName",      map_draw_spawn_guild_name);
    map_draw_spawn_location         = pt.get<bool>("Draw.bSpawnLocation",       map_draw_spawn_location);
    map_draw_spawn_heading          = pt.get<bool>("Draw.bSpawnHeading",        map_draw_spawn_heading);
    map_draw_spawn_distance         = pt.get<bool>("Draw.bSpawnDistance",       map_draw_spawn_distance);
    map_draw_spawn_distance_z       = pt.get<bool>("Draw.bSpawnDistanceZ",      map_draw_spawn_distance_z);
    map_draw_spawn_movement_speed   = pt.get<bool>("Draw.bSpawnMovementSpeed",  map_draw_spawn_movement_speed);
    map_draw_spawn_standing_state   = pt.get<bool>("Draw.bSpawnStandingState",  map_draw_spawn_standing_state);
    map_draw_spawn_is_holding       = pt.get<bool>("Draw.bSpawnIsHolding",      map_draw_spawn_is_holding);  

    spawn_filter_enabled             = pt.get<bool>("SpawnFilter.bEnabled",           spawn_filter_enabled);
    spawn_filter_always_show_players = pt.get<bool>("SpawnFilter.bAlwaysShowPlayers", spawn_filter_always_show_players);
    spawn_filter_name                = pt.get<std::string>("SpawnFilter.sName",       spawn_filter_name);

    if (spawn_filter_name.size())
    {
        boost::split(spawn_filter_name_data, spawn_filter_name, boost::is_any_of(","));
    }
    else
    {
        spawn_filter_enabled = false;
    }

    spawn_line_enabled = pt.get<bool>("SpawnLine.bEnabled",     spawn_line_enabled);
    spawn_line_name    = pt.get<std::string>("SpawnLine.sName", spawn_line_name);

    if (spawn_line_name.size())
    {
        boost::split(spawn_line_name_data, spawn_line_name, boost::is_any_of(","));
    }
    else
    {
        spawn_line_enabled = false;
    }
}

void draw_bitmap_characters(float x, float y, void *font, std::string text)
{
    glRasterPos2f(x, y);

    for (unsigned int i = 0; i < text.size(); i++)
    {
        glutBitmapCharacter(font, text[i]);
    }
}

void draw_bitmap_string(float x, float y, void *font, std::string text)
{
    glRasterPos2f(x, y);

    glutBitmapString(font, (const unsigned char*)text.c_str());
}

void draw_line(float from_x, float from_y, float to_x, float to_y)
{
    glBegin(GL_LINES);
        glVertex2f(from_x, from_y);
        glVertex2f(to_x,   to_y);
    glEnd();
}

void draw_plus(float x, float y, int size)
{
    glBegin(GL_LINES);
        glVertex2f(x - size, y);
        glVertex2f(x + size, y);

        glVertex2f(x, y - size);
        glVertex2f(x, y + size);
    glEnd();
}

void draw_cross(float x, float y, int size)
{
    glBegin(GL_LINES);
        glVertex2f(x - size, y - size);
        glVertex2f(x + size, y + size);

        glVertex2f(x - size, y + size);
        glVertex2f(x + size, y - size);
    glEnd();
}

void draw_square(float x, float y, int size)
{
    size = size / 2;

    float x1 = x - size;
    float y1 = y - size;

    float x2 = x + size;
    float y2 = y + size;

    glBegin(GL_QUADS);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
    glEnd();
}

void draw_arrow_by_heading(float x, float y, float heading, int size, bool has_arrow_head)
{
    float zoom = MAP_ZOOM_DEFAULT;

    if (map_zoom < MAP_ZOOM_DEFAULT)
    {
        zoom = map_zoom;
    }
    else
    {
        zoom = MAP_ZOOM_DEFAULT;
    }




    float heading_radians;

    float arrow_add_y, arrow_add_x;
    float arrow_y, arrow_x;

    int arrow_length;




    glBegin(GL_LINES);




    heading = (heading * 360) / 512; // convert from 512 degrees to 360 degrees

    heading -= 90; // rotate by -90 degrees

    if (heading < 0)
    {
        heading += 360;
    }

    heading_radians = heading * PI / 180; // convert degrees to radians

    arrow_add_y = sin(heading_radians);
    arrow_add_x = cos(heading_radians);

    arrow_y = y;
    arrow_x = x;

    arrow_length = size / zoom;

    arrow_y += arrow_add_y * arrow_length;
    arrow_x -= arrow_add_x * arrow_length;

    glVertex2f(x, y);
    glVertex2f(arrow_x, arrow_y);




    if (has_arrow_head == false)
    {
        glEnd();
        return;
    }




    float heading_ex, heading_radians_ex;

    float arrow_add_y_ex, arrow_add_x_ex;
    float arrow_y_ex, arrow_x_ex;

    int arrow_length_ex;




    heading_ex = heading;

    heading_ex -= 30;

    if (heading_ex < 0)
    {
        heading_ex += 360;
    }

    heading_radians_ex = heading_ex * PI / 180; // convert degrees to radians

    arrow_add_y_ex = sin(heading_radians_ex);
    arrow_add_x_ex = cos(heading_radians_ex);

    arrow_y_ex = arrow_y;
    arrow_x_ex = arrow_x;

    arrow_length_ex = (size / 2) / zoom;

    arrow_y_ex -= arrow_add_y_ex * arrow_length_ex;
    arrow_x_ex += arrow_add_x_ex * arrow_length_ex;

    glVertex2f(arrow_x, arrow_y);
    glVertex2f(arrow_x_ex, arrow_y_ex);



    heading_ex = heading;

    heading_ex += 30;

    if (heading_ex > 360)
    {
        heading_ex = heading_ex - 360;
    }

    heading_radians_ex = heading_ex * PI / 180; // convert degrees to radians

    arrow_add_y_ex = sin(heading_radians_ex);
    arrow_add_x_ex = cos(heading_radians_ex);

    arrow_y_ex = arrow_y;
    arrow_x_ex = arrow_x;

    arrow_length_ex = (size / 2) / zoom;

    arrow_y_ex -= arrow_add_y_ex * arrow_length_ex;
    arrow_x_ex += arrow_add_x_ex * arrow_length_ex;

    glVertex2f(arrow_x, arrow_y);
    glVertex2f(arrow_x_ex, arrow_y_ex);




    glEnd();
}

void draw_spawn_list()
{
    int player_spawn_info = everquest_get_player_spawn_info(memory);

    int spawn_info_address = player_spawn_info;

    int spawn_next_spawn_info = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER, 4);

    spawn_info_address = spawn_next_spawn_info;

    spawn_list.clear();

    int spawn_name_bitmap_length_max = 0;

    for (int i = 0; i < EVERQUEST_SPAWNS_MAX; i++)
    {
        spawn_next_spawn_info = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER, 4);

        if (spawn_next_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
        {
            break;
        }

        map_spawn_t map_spawn;

        map_spawn.count = 1;

        std::string spawn_name = memory.read_string(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NAME, 0x40);

        spawn_name.erase(boost::remove_if(spawn_name, boost::is_any_of("0123456789")), spawn_name.end());

        boost::replace_all(spawn_name, "_", " ");

        map_spawn.name = spawn_name;

        map_spawn.type = memory.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_TYPE);

        if (spawn_name.size())
        {
            spawn_list_ret = spawn_list.insert(std::pair<std::string, map_spawn_t>(spawn_name, map_spawn));

            if (spawn_list_ret.second == false)
            {
                spawn_list_ret.first->second.count++;
            }

            int spawn_name_bitmap_length = glutBitmapLength(font_name, (unsigned char*)spawn_name.c_str());

            if (spawn_name_bitmap_length > spawn_name_bitmap_length_max)
            {
                spawn_name_bitmap_length_max = spawn_name_bitmap_length;
            }
        }

        spawn_info_address = spawn_next_spawn_info;
    }

    if (!spawn_list.size())
    {
        return;
    }

    int offset_x = font_size;
    int offset_y = font_size;

    int spawn_count_total = 0;

    for (spawn_list_it = spawn_list.begin(); spawn_list_it != spawn_list.end(); spawn_list_it++)
    {
        std::stringstream spawn_list_text;
        spawn_list_text << (*spawn_list_it).second.name;

        int spawn_count = (*spawn_list_it).second.count;

        spawn_count_total += spawn_count;

        if (spawn_count > 1)
        {
            spawn_list_text << " " << "(" << spawn_count << ")";
        }

        BYTE spawn_type = (*spawn_list_it).second.type;

        switch (spawn_type)
        {
            case EVERQUEST_SPAWN_INFO_TYPE_PLAYER:
                glColor3ub(255, 0, 0);
                break;

            case EVERQUEST_SPAWN_INFO_TYPE_NPC:
                glColor3ub(255, 255, 255);
                break;

            case EVERQUEST_SPAWN_INFO_TYPE_CORPSE:
                glColor3ub(255, 255, 0);
                break;

            default:
                glColor3ub(255, 255, 255);
        }

        draw_bitmap_string(offset_x, offset_y, font_name, spawn_list_text.str());

        offset_y += font_size;

        if (offset_y > window_height)
        {
            offset_x += spawn_name_bitmap_length_max + font_size;
            offset_y = font_size;
        }

        spawn_list_it_ex = spawn_list_it;

        spawn_list_it_ex++;

        if (spawn_list_it_ex == spawn_list.end())
        {
            std::stringstream spawn_count_text;
            spawn_count_text << "Total Spawns: " << spawn_count_total;

            glColor3ub(255, 0, 255);

            draw_bitmap_string(offset_x, offset_y, font_name, spawn_count_text.str());
        }
    }
}

void spawn_filter_toggle()
{
    toggle_bool(spawn_filter_enabled);
}

void spawn_line_toggle()
{
    toggle_bool(spawn_line_enabled);
}

void window_always_on_top_toggle()
{
    toggle_bool(window_always_on_top);

    set_window_always_on_top(window_hwnd, window_always_on_top);
}

void map_draw_info_text_toggle()
{
    toggle_bool(map_draw_info_text);
}

void map_draw_spawn_list_toggle()
{
    toggle_bool(map_draw_spawn_list);
}

void map_draw_lines_toggle()
{
    toggle_bool(map_draw_lines);
}

void map_draw_points_toggle()
{
    toggle_bool(map_draw_points);
}

void map_draw_spawns_toggle()
{
    toggle_bool(map_draw_spawns);
}

void map_draw_spawn_type_player_toggle()
{
    toggle_bool(map_draw_spawn_type_player);
}

void map_draw_spawn_type_npc_toggle()
{
    toggle_bool(map_draw_spawn_type_npc);
}

void map_draw_spawn_type_corpse_toggle()
{
    toggle_bool(map_draw_spawn_type_corpse);
}

void map_center()
{
    map_offset_x = 0;
    map_offset_y = 0;
}

void map_scroll_up()
{
    map_offset_y += map_offset_multiplier * map_zoom;
}

void map_scroll_down()
{
    map_offset_y -= map_offset_multiplier * map_zoom;
}

void map_scroll_left()
{
    map_offset_x += map_offset_multiplier * map_zoom;
}

void map_scroll_right()
{
    map_offset_x -= map_offset_multiplier * map_zoom;
}

void map_zoom_reset()
{
    map_zoom = MAP_ZOOM_DEFAULT;
}

void map_zoom_in()
{
    map_zoom -= map_zoom_multiplier * map_zoom;
}

void map_zoom_out()
{
    map_zoom += map_zoom_multiplier * map_zoom;
}

void map_max_z_increase()
{
    if (map_max_z_positive < 0 || map_max_z_negative < 0)
    {
        map_max_z_positive = 0;
        map_max_z_negative = 0;
    }

    if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
    {
        map_max_z_positive += 1;
        map_max_z_negative += 1;
    }
    else if (glutGetModifiers() & GLUT_ACTIVE_CTRL)
    {
        map_max_z_positive += 1;
    }
    else if (glutGetModifiers() & GLUT_ACTIVE_ALT)
    {
        map_max_z_negative += 1;
    }
    else
    {
        map_max_z_positive += map_max_z_multiplier;
        map_max_z_negative += map_max_z_multiplier;
    }
}

void map_max_z_decrease()
{
    if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
    {
        map_max_z_positive -= 1;
        map_max_z_negative -= 1;
    }
    else if (glutGetModifiers() & GLUT_ACTIVE_CTRL)
    {
        map_max_z_positive -= 1;
    }
    else if (glutGetModifiers() & GLUT_ACTIVE_ALT)
    {
        map_max_z_negative -= 1;
    }
    else
    {
        map_max_z_positive -= map_max_z_multiplier;
        map_max_z_negative -= map_max_z_multiplier;
    }

    if (map_max_z_positive < 0 || map_max_z_negative < 0)
    {
        map_max_z_positive = 0;
        map_max_z_negative = 0;
    }
}

void map_max_spawn_distance_increase()
{
    if (map_max_spawn_distance < 0)
    {
        map_max_spawn_distance = 0;
    }

    if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
    {
        map_max_spawn_distance += 1;
    }
    else
    {
        map_max_spawn_distance += map_max_spawn_distance_multiplier;
    }
}

void map_max_spawn_distance_decrease()
{
    if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
    {
        map_max_spawn_distance -= 1;
    }
    else
    {
        map_max_spawn_distance -= map_max_spawn_distance_multiplier;
    }

    if (map_max_spawn_distance < 0)
    {
        map_max_spawn_distance = 0;
    }
}

void map_max_spawn_distance_z_increase()
{
    if (map_max_spawn_distance_z < 0)
    {
        map_max_spawn_distance_z = 0;
    }

    if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
    {
        map_max_spawn_distance_z += 1;
    }
    else
    {
        map_max_spawn_distance_z += map_max_spawn_distance_multiplier;
    }
}

void map_max_spawn_distance_z_decrease()
{
    if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
    {
        map_max_spawn_distance_z -= 1;
    }
    else
    {
        map_max_spawn_distance_z -= map_max_spawn_distance_multiplier;
    }

    if (map_max_spawn_distance_z < 0)
    {
        map_max_spawn_distance_z = 0;
    }
}

void update_process(int value)
{
    HWND everquest_hwnd = FindWindow(NULL, everquest_title.c_str());

    memory.set_process_by_hwnd(everquest_hwnd);

    glutTimerFunc(100, update_process, 0);
}

void update_zone(int value)
{
    glutTimerFunc(1000, update_zone, 0);

    if (memory.get_process_hwnd() == NULL)
    {
        glutSetWindowTitle(application_name.c_str());
        return;
    }

    std::string zone_short_name = everquest_get_zone_short_name(memory);

    boost::algorithm::to_lower(zone_short_name);

    std::string zone_long_name = everquest_get_zone_long_name(memory);

    std::stringstream window_title_buffer;
    window_title_buffer << zone_long_name << " " << "(" << zone_short_name << ")" << " - " << application_name;

    glutSetWindowTitle(window_title_buffer.str().c_str());

    if (zone_short_name == map_zone)
    {
        return;
    }

    map_zone.assign(zone_short_name);

    map_zone_ini_file.str("");
    map_zone_ini_file << "maps/" << map_zone << ".ini";

    parse_ini(map_zone_ini_file.str());

    std::stringstream map_filename_buffer;
    map_filename_buffer << map_folder << "/" << map_zone << ".txt";

    std::fstream file;
    file.open(map_filename_buffer.str().c_str(), std::ios::in);

    if (!file.is_open())
    {
        return;
    }

    map_lines.clear();
    map_points.clear();

    std::string line;
    std::vector<std::string> line_data;

    if (file.is_open())
    {
        while (file.good())
        {
            std::getline(file, line);

            boost::replace_all(line, " ", "");

            if (!line.size())
            {
                continue;
            }

            char line_type = line.at(0);

            line = line.substr(1);

            boost::split(line_data, line, boost::is_any_of(","));

            if (!line_data.size())
            {
                continue;
            }

            if (line_type == 'L')
            {
                map_line_t map_line;

                map_line.from_x = boost::lexical_cast<float>(line_data.at(0));
                map_line.from_y = boost::lexical_cast<float>(line_data.at(1));
                map_line.from_z = boost::lexical_cast<float>(line_data.at(2));

                map_line.to_x = boost::lexical_cast<float>(line_data.at(3));
                map_line.to_y = boost::lexical_cast<float>(line_data.at(4));
                map_line.to_z = boost::lexical_cast<float>(line_data.at(5));

                map_line.r = boost::lexical_cast<int>(line_data.at(6));
                map_line.g = boost::lexical_cast<int>(line_data.at(7));
                map_line.b = boost::lexical_cast<int>(line_data.at(8));

                map_lines.push_back(map_line);
            }

            if (line_type == 'P')
            {
                map_point_t map_point;

                map_point.x = boost::lexical_cast<float>(line_data.at(0));
                map_point.y = boost::lexical_cast<float>(line_data.at(1));
                map_point.z = boost::lexical_cast<float>(line_data.at(2));

                map_point.r = boost::lexical_cast<int>(line_data.at(3));
                map_point.g = boost::lexical_cast<int>(line_data.at(4));
                map_point.b = boost::lexical_cast<int>(line_data.at(5));

                map_point.size = boost::lexical_cast<int>(line_data.at(6));

                map_point.text = line_data.at(7);

                boost::replace_all(map_point.text, "_", " ");

                map_points.push_back(map_point);
            }
        }
    }

    file.close();

    map_origin_x = window_width  / 2;
    map_origin_y = window_height / 2;

    //map_zoom_reset();
    map_center();
}

void update_spawns(int value)
{
    if (memory.get_process_hwnd() == NULL)
    {
        return;
    }

    map_spawns.clear();

    int player_spawn_info = everquest_get_player_spawn_info(memory);
    int target_spawn_info = everquest_get_target_spawn_info(memory);

    float player_y = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
    float player_x = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
    float player_z = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

    int spawn_info_address = player_spawn_info;

    int spawn_next_spawn_info = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER, 4);

    spawn_info_address = spawn_next_spawn_info;

    for (int i = 0; i < EVERQUEST_SPAWNS_MAX; i++)
    {
        spawn_next_spawn_info = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER, 4);

        if (spawn_next_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
        {
            break;
        }

        int spawn_actor_info = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER, 4);

        map_spawn_t map_spawn;

        map_spawn.address = spawn_info_address;

        map_spawn.spawn_id = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_SPAWN_ID, 2);
        map_spawn.owner_id = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_OWNER_ID, 2);

        map_spawn.name      = memory.read_string(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NAME,      0x40);
        map_spawn.last_name = memory.read_string(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_LAST_NAME, 0x20);

        map_spawn.y = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Y);
        map_spawn.x = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_X);
        map_spawn.z = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Z);

        map_spawn.distance = calculate_distance(player_x, player_y, map_spawn.x, map_spawn.y);

        map_spawn.distance_z = map_spawn.z - player_z;

        map_spawn.heading = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HEADING);

        map_spawn.movement_speed = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_MOVEMENT_SPEED);

        map_spawn.standing_state = memory.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_STANDING_STATE);

        map_spawn.type   = memory.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_TYPE);
        map_spawn.level  = memory.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_LEVEL);
        map_spawn.race   = memory.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_RACE);
        map_spawn._class = memory.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_CLASS);

        map_spawn.hp     = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HP_CURRENT, 4);
        map_spawn.hp_max = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HP_MAX,     4);

        map_spawn.is_target = (spawn_info_address == target_spawn_info ? true : false);

        map_spawn.is_player_corpse = false;

        if (map_spawn.type == EVERQUEST_SPAWN_INFO_TYPE_CORPSE)
        {
            std::string player_name = memory.read_string(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_NAME, 0x40);

            std::size_t found = map_spawn.name.find(player_name);

            if (found != std::string::npos)
            {
                map_spawn.is_player_corpse = true;
            }
        }

        map_spawn.is_holding_both      = memory.read_bytes(spawn_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_BOTH,      4);
        map_spawn.is_holding_secondary = memory.read_bytes(spawn_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_SECONDARY, 4);
        map_spawn.is_holding_primary   = memory.read_bytes(spawn_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_PRIMARY,   4);

        map_spawns.push_back(map_spawn);

        spawn_info_address = spawn_next_spawn_info;
    }

    glutTimerFunc(100, update_spawns, 0);
}

void keyboard(unsigned char key, int x, int y)
{
    //std::stringstream key_buffer;
    //key_buffer << "Key: " << (int)key;
    //MessageBox(NULL, key_buffer.str().c_str(), "key", MB_OK | MB_ICONINFORMATION);

    switch (key)
    {
        case 27: // Escape
            glutDestroyWindow(window_id);
            exit(0);
            break;

        case 8: // Backspace
            //glutFullScreenToggle(); // crashes eqw
            map_draw_spawn_list_toggle();
            break;

        case 32: // Space
            map_zoom_reset();
            map_center();
            break;

        case 119: // w
            map_scroll_up();
            break;

        case 115: // s
            map_scroll_down();
            break;

        case 97:  // a
            map_scroll_left();
            break;

        case 100: // d
            map_scroll_right();
            break;

        case 48: // 0
            map_center();
            break;

        case 49: // 1
            map_zoom = 1.0;
            break;

        case 50: // 2
            map_zoom = 2.0;
            break;

        case 51: // 3
            map_zoom = 3.0;
            break;

        case 52: // 4
            map_zoom = 4.0;
            break;

        case 53: // 5
            map_zoom = 5.0;
            break;

        case 54: // 6
            map_zoom = 6.0;
            break;

        case 55: // 7
            map_zoom = 7.0;
            break;

        case 56: // 8
            map_zoom = 8.0;
            break;

        case 57: // 9
            map_zoom = 9.0;
            break;

        case 13: // Enter
            map_zoom_reset();
            break;

        case 43: // Numpad Add
        case 61: // = +
            map_zoom_in();
            break;

        case 45: // Numpad Subtract or - _
            map_zoom_out();
            break;

        case ',':
            map_max_spawn_distance_decrease();
            break;

        case '.':
            map_max_spawn_distance_increase();
            break;

        case 91: // { [
            map_max_spawn_distance_z_decrease();
            break;

        case 93: // } ]
            map_max_spawn_distance_z_increase();
            break;

        case 127: // Delete
            map_max_z_decrease();
            break;
    }
}

void hotkeys(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_F1:
            map_draw_info_text_toggle();
            break;

        case GLUT_KEY_F2:
            map_draw_lines_toggle();
            break;

        case GLUT_KEY_F3:
            map_draw_points_toggle();
            break;

        case GLUT_KEY_F4:
            map_draw_spawns_toggle();
            break;

        case GLUT_KEY_F5:
            parse_ini(ini_file);
            parse_ini(map_zone_ini_file.str());
            break;

        case GLUT_KEY_F6:
            map_draw_spawn_type_player_toggle();
            break;

        case GLUT_KEY_F7:
            map_draw_spawn_type_npc_toggle();
            break;

        case GLUT_KEY_F8:
            map_draw_spawn_type_corpse_toggle();
            break;

        case GLUT_KEY_F9:
            map_draw_spawn_list_toggle();
            break;

        case GLUT_KEY_F10:
            spawn_filter_toggle();
            break;

        case GLUT_KEY_F11:
            //glutFullScreenToggle(); // crashes eqw
            spawn_line_toggle();
            break;

        //case GLUT_KEY_F12: // broken, does not work
            //window_always_on_top_toggle();
            //break;

        case GLUT_KEY_UP:
            map_scroll_up();
            break;

        case GLUT_KEY_DOWN:
            map_scroll_down();
            break;

        case GLUT_KEY_LEFT:
            map_scroll_left();
            break;

        case GLUT_KEY_RIGHT:
            map_scroll_right();
            break;

        case GLUT_KEY_HOME:
            map_center();
            break;

        case GLUT_KEY_END:
            map_zoom_reset();
            break;

        case GLUT_KEY_PAGE_UP:
            map_zoom_in();
            break;

        case GLUT_KEY_PAGE_DOWN:
            map_zoom_out();
            break;

        case GLUT_KEY_INSERT:
            map_max_z_increase();
            break;

        //case GLUT_KEY_DELETE: // broken, does not work
            //map_max_z_decrease();
            //break;
    }  
}

void menu(int value)
{
    menu_value = value;

    switch (value)
    {
        case MENU_APP_EXIT:
            glutDestroyWindow(window_id);
            exit(0);
            break;

        case MENU_MAP_DRAW_INFO_TEXT_TOGGLE:
            map_draw_info_text_toggle();
            break;

        case MENU_MAP_DRAW_LINES_TOGGLE:
            map_draw_lines_toggle();
            break;

        case MENU_MAP_DRAW_POINTS_TOGGLE:
            map_draw_points_toggle();
            break;

        case MENU_MAP_DRAW_SPAWNS_TOGGLE:
            map_draw_spawns_toggle();
            break;

        case MENU_MAP_DRAW_SPAWN_LIST_TOGGLE:
            map_draw_spawn_list_toggle();
            break;
    }

    glutPostRedisplay();
} 
void menu_create()
{
    menu_id = glutCreateMenu(menu);

    glutSetMenu(menu_id);

    //glutSetMenuFont(menu_id, font_name); broken

    glutAddMenuEntry("Information Text (F1)",                    MENU_MAP_DRAW_INFO_TEXT_TOGGLE);
    glutAddMenuEntry("Lines            (F2)",                    MENU_MAP_DRAW_LINES_TOGGLE);
    glutAddMenuEntry("Points           (F3)",                    MENU_MAP_DRAW_POINTS_TOGGLE);
    glutAddMenuEntry("Spawns           (F4)",                    MENU_MAP_DRAW_SPAWNS_TOGGLE);
    glutAddMenuEntry("----------------------------", -1);
    glutAddMenuEntry("Spawn List       (Backspace)",             MENU_MAP_DRAW_SPAWN_LIST_TOGGLE);
    glutAddMenuEntry("----------------------------", -1);
    glutAddMenuEntry("Exit",                                     MENU_APP_EXIT);

    menu_num_items = glutGet(GLUT_MENU_NUM_ITEMS);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void menu_destroy()
{
    glutDestroyMenu(menu_id);
}

void menu_recreate()
{
    menu_destroy();
    menu_create();
}

void menu_status(int status, int x, int y)
{
    if (status == GLUT_MENU_IN_USE)
    {
        menu_in_use = true;
    }
    else
    {
        menu_in_use = false;
    }
}

void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_UP)
    {
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        mouse_dragging = false;
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        //glutSetCursor(GLUT_CURSOR_CYCLE);

        SetCursor(LoadCursor(NULL, IDC_SIZEALL));

        mouse_dragging_start_x = x;
        mouse_dragging_start_y = y;

        mouse_dragging = true;
    }

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        //glutSetCursor(GLUT_CURSOR_CROSSHAIR);

        SetCursor(LoadCursor(NULL, IDC_HAND));
    }

/*
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
        map_offset_x += (map_origin_x - x) * map_zoom;
        map_offset_y += (map_origin_y - y) * map_zoom;
    }
*/

    if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
    {
        map_zoom_reset();
        map_center();
    }
}

void mouse_wheel(int button, int direction, int x, int y)
{
    if (direction > 0)
    {
        map_zoom_in();
    }
    else
    {
        map_zoom_out();
    }
}

void motion(int x, int y)
{
    if (mouse_dragging == true)
    {
        map_offset_x += (x - mouse_dragging_start_x) * map_zoom;
        map_offset_y += (y - mouse_dragging_start_y) * map_zoom;

        mouse_dragging_start_x = x;
        mouse_dragging_start_y = y;
    }
}

void reshape(int w, int h)
{
    window_width  = w;
    window_height = h;

    map_origin_x = window_width  / 2;
    map_origin_y = window_height / 2;

    glViewport(0, 0, window_width, window_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, window_width, window_height, 0, 0, 1);
}

void render()
{
    Sleep(1);

    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (map_draw_spawn_list == true)
    {
        glColor3ub(255, 255, 255);

        draw_spawn_list();

        glutSwapBuffers();

        return;
    }

    draw_time_start = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    int player_spawn_info = everquest_get_player_spawn_info(memory);

    float player_y = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
    float player_x = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
    float player_z = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

    float player_heading = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING);

    float player_movement_speed = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_MOVEMENT_SPEED);

    map_draw_x = player_x;
    map_draw_y = player_y;

    if (map_center_on_target == true)
    {
        int target_spawn_info = everquest_get_target_spawn_info(memory);

        if (target_spawn_info != player_spawn_info)
        {
            if (target_spawn_info != EVERQUEST_SPAWN_INFO_NULL)
            {
                float target_y = memory.read_any<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
                float target_x = memory.read_any<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
                //float target_z = memory.read_any<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

                map_draw_x = target_x;
                map_draw_y = target_y;
            }
        }
    }

    player_y = reverse_sign(player_y);
    player_x = reverse_sign(player_x);

    float player_map_x = ((player_x / map_zoom) + map_origin_x) + ((map_draw_x + map_offset_x) / map_zoom);
    float player_map_y = ((player_y / map_zoom) + map_origin_y) + ((map_draw_y + map_offset_y) / map_zoom);

    if (map_draw_lines == true)
    {
        glBegin(GL_LINES);

        num_lines = 0;

        foreach (map_line_t map_line, map_lines)
        {
            if (map_max_z_positive > 0)
            {
                if
                (
                    map_line.from_z > player_z + map_max_z_positive ||
                    map_line.to_z   > player_z + map_max_z_positive
                )
                {
                    continue;
                }
            }

            if (map_max_z_negative > 0)
            {
                if
                (
                    map_line.from_z < player_z - map_max_z_negative ||
                    map_line.to_z   < player_z - map_max_z_negative
                )
                {
                    continue;
                }
            }

            float line_start_x = map_line.from_x;
            float line_start_y = map_line.from_y;

            float line_stop_x  = map_line.to_x;
            float line_stop_y  = map_line.to_y;

            float line_map_start_x = ((line_start_x / map_zoom) + map_origin_x) + ((map_draw_x + map_offset_x) / map_zoom);
            float line_map_start_y = ((line_start_y / map_zoom) + map_origin_y) + ((map_draw_y + map_offset_y) / map_zoom);

            float line_map_stop_x = ((line_stop_x / map_zoom) + map_origin_x) + ((map_draw_x + map_offset_x) / map_zoom);
            float line_map_stop_y = ((line_stop_y / map_zoom) + map_origin_y) + ((map_draw_y + map_offset_y) / map_zoom);

            float line_map_distance_start = calculate_distance(map_origin_x, map_origin_y, line_map_start_x, line_map_start_y);
            float line_map_distance_stop  = calculate_distance(map_origin_x, map_origin_y, line_map_stop_x,  line_map_stop_y);

/*
            float line_midpoint_x = (line_start_x + line_stop_x) / 2;
            float line_midpoint_y = (line_start_y + line_stop_y) / 2;

            float line_map_midpoint_x = ((line_midpoint_x / map_zoom) + map_origin_x) + ((map_draw_x + map_offset_x) / map_zoom);
            float line_map_midpoint_y = ((line_midpoint_y / map_zoom) + map_origin_y) + ((map_draw_y + map_offset_y) / map_zoom);
*/

            if
            (
                line_map_distance_start > map_origin_x &&
                line_map_distance_stop  > map_origin_x &&
                line_map_distance_start > map_origin_y &&
                line_map_distance_stop  > map_origin_y
            )
            {
                continue;
            }

            if (map_line.r == 0 && map_line.g == 0 && map_line.b == 0)
            {
                map_line.r = 255;
                map_line.g = 255;
                map_line.b = 255;
            }

            glColor3ub(map_line.r, map_line.g, map_line.b);

            glVertex2f(line_map_start_x, line_map_start_y);
            glVertex2f(line_map_stop_x,  line_map_stop_y);

            num_lines++;
        }

        num_lines_ex = num_lines;

        glEnd();
    }

    if (map_draw_points == true)
    {
        num_points = 0;

        foreach (map_point_t map_point, map_points)
        {
            float point_map_x = ((map_point.x / map_zoom) + map_origin_x) + ((map_draw_x + map_offset_x) / map_zoom);
            float point_map_y = ((map_point.y / map_zoom) + map_origin_y) + ((map_draw_y + map_offset_y) / map_zoom);

            float point_map_distance = calculate_distance(map_origin_x, map_origin_y, point_map_x, point_map_y);

            if (point_map_distance > map_origin_x && point_map_distance > map_origin_x)
            {
                continue;
            }

            if (map_point.r == 0 && map_point.g == 0 && map_point.b == 0)
            {
                map_point.r = 255;
                map_point.g = 255;
                map_point.b = 255;
            }

            glColor3ub(map_point.r, map_point.g, map_point.b);

            draw_plus(point_map_x, point_map_y, 4);

            draw_bitmap_string(point_map_x, point_map_y + (font_size * 1.5), font_name, map_point.text);

            num_points++;
        }

        num_points_ex = num_points;
    }

    if (map_draw_spawns == true)
    {
        num_spawns = 0;

        foreach (map_spawn_t map_spawn, map_spawns)
        {
            if (map_draw_spawn_type_player == false)
            {
                if (map_spawn.type == EVERQUEST_SPAWN_INFO_TYPE_PLAYER)
                {
                    continue;
                }
            }

            if (map_draw_spawn_type_npc == false)
            {
                if (map_spawn.type == EVERQUEST_SPAWN_INFO_TYPE_NPC)
                {
                    continue;
                }
            }

            if (map_draw_spawn_type_corpse == false)
            {
                if (map_spawn.type == EVERQUEST_SPAWN_INFO_TYPE_CORPSE)
                {
                    continue;
                }
            }

            if (map_max_spawn_distance > 0)
            {
                if (map_spawn.distance > map_max_spawn_distance)
                {
                    continue;
                }
            }

            if (map_max_spawn_distance_z > 0)
            {
                if (abs(map_spawn.distance_z) > map_max_spawn_distance_z)
                {
                    continue;
                }
            }

            float spawn_y = reverse_sign(map_spawn.y);
            float spawn_x = reverse_sign(map_spawn.x);

            float spawn_map_x = ((spawn_x / map_zoom) + map_origin_x) + ((map_draw_x + map_offset_x) / map_zoom);
            float spawn_map_y = ((spawn_y / map_zoom) + map_origin_y) + ((map_draw_y + map_offset_y) / map_zoom);

            float spawn_map_distance = calculate_distance(map_origin_x, map_origin_y, spawn_map_x, spawn_map_y);

            if (spawn_map_distance > map_origin_x && spawn_map_distance > map_origin_y)
            {
                if (map_spawn.is_target != true)
                {
                    continue;
                }
            }

            if (spawn_filter_enabled == true)
            {
                if (spawn_filter_name_data.size())
                {
                    bool found_spawn = false;

                    foreach(std::string spawn_filter_name_data_value, spawn_filter_name_data)
                    {
                        std::size_t found = map_spawn.name.find(spawn_filter_name_data_value);

                        if (found != std::string::npos)
                        {
                            found_spawn = true;
                        }
                    }

                    if (spawn_filter_always_show_players == true)
                    {
                        if (map_spawn.type == EVERQUEST_SPAWN_INFO_TYPE_PLAYER)
                        {
                            found_spawn = true;
                        }
                    }

                    if (found_spawn == false)
                    {
                        continue;
                    }
                }
            }

            switch (map_spawn.type)
            {
                case EVERQUEST_SPAWN_INFO_TYPE_PLAYER:
                    glColor3ub(255, 0, 0);
                    draw_square(spawn_map_x, spawn_map_y, 4);
                    break;

                case EVERQUEST_SPAWN_INFO_TYPE_NPC:
                {
                    if (map_spawn.movement_speed > 0)
                    {
                        glColor3ub(128, 128, 128);
                    }
                    else
                    {
                        glColor3ub(64, 64, 64);
                    }

                    draw_plus(spawn_map_x, spawn_map_y, 4);

                    break;
                }

                case EVERQUEST_SPAWN_INFO_TYPE_CORPSE:
                {
                    if (map_spawn.is_player_corpse == true)
                    {
                        glColor3ub(255, 0, 255);

                        if (map_draw_player_corpse_line == true)
                        {
                            draw_line(player_map_x, player_map_y, spawn_map_x, spawn_map_y);
                        }
                    }
                    else
                    {
                        glColor3ub(255, 255, 0);
                    }

                    draw_cross(spawn_map_x, spawn_map_y, 4);

                    break;
                }

                default:
                    glColor3ub(64, 64, 64);
                    draw_plus(spawn_map_x, spawn_map_y, 4);
            }

            if (map_draw_spawn_name == true)
            {
                bool draw_spawn_name = true;

                if (num_spawns_ex > map_max_spawn_names_visible)
                {
                    draw_spawn_name = false;
                }

                if (map_spawn.is_target == true)
                {
                    draw_spawn_name = true;
                }

                if (draw_spawn_name == true)
                {
                    std::stringstream spawn_name_text;

                    spawn_name_text << map_spawn.name;

                    if (map_draw_spawn_last_name == true)
                    {
                        if (map_spawn.last_name.size())
                        {
                            spawn_name_text << " " << map_spawn.last_name;
                        }
                    }

                    draw_bitmap_string(spawn_map_x, spawn_map_y + (font_size * 1.5), font_name, spawn_name_text.str());

                    std::vector<std::string> spawn_extra_text;

                    if (map_spawn.type == EVERQUEST_SPAWN_INFO_TYPE_PLAYER)
                    {
                        if (map_draw_spawn_level_race_class == true)
                        {
                            std::stringstream buffer;

                            buffer << "L" << map_spawn.level;
                            buffer << " ";
                            buffer << everquest_get_race_short_name(map_spawn.race);
                            buffer << " ";
                            buffer << everquest_get_class_short_name(map_spawn._class);

                            spawn_extra_text.push_back(buffer.str());
                        }

                        if (map_draw_spawn_guild_name == true)
                        {
                            // TODO
                        }
                    }

                    if (map_draw_spawn_location == true)
                    {
                        std::stringstream buffer;
                        buffer << "Loc: " << map_spawn.y << ", " << map_spawn.x << ", " << map_spawn.z;

                        spawn_extra_text.push_back(buffer.str());
                    }

                    if (map_draw_spawn_heading == true)
                    {
                        std::stringstream buffer;
                        buffer << "Heading: " << map_spawn.heading;

                        spawn_extra_text.push_back(buffer.str());
                    }

                    if (map_draw_spawn_distance == true)
                    {
                        std::stringstream buffer;
                        buffer << "Distance: " << map_spawn.distance;

                        spawn_extra_text.push_back(buffer.str());
                    }

                    if (map_draw_spawn_distance_z == true)
                    {
                        std::stringstream buffer;
                        buffer << "Distance Z: " << map_spawn.distance_z;

                        spawn_extra_text.push_back(buffer.str());
                    }

                    if (map_draw_spawn_movement_speed == true)
                    {
                        if (map_spawn.movement_speed > 0)
                        {
                            std::stringstream buffer;
                            buffer << "Speed: " << map_spawn.movement_speed;

                            spawn_extra_text.push_back(buffer.str());
                        }
                    }

                    if (map_draw_spawn_standing_state == true)
                    {
                        std::stringstream buffer;
                        buffer << "Standing State: " << map_spawn.standing_state;

                        // TODO

                        spawn_extra_text.push_back(buffer.str());
                    }

                    if (map_draw_spawn_is_holding == true)
                    {
                        if (map_spawn.is_holding_primary != 0 || map_spawn.is_holding_secondary != 0)
                        {
                            std::stringstream buffer;

                            buffer << "Is Holding: ";
                            buffer << "P" << map_spawn.is_holding_primary;
                            buffer << " ";
                            buffer << "S" << map_spawn.is_holding_secondary;

                            spawn_extra_text.push_back(buffer.str());
                        }
                    }

                    int spawn_extra_text_index = 1;
                    foreach (std::string spawn_extra_text_value, spawn_extra_text)
                    {
                        draw_bitmap_string(spawn_map_x, spawn_map_y + (font_size * 1.5) + (font_size * spawn_extra_text_index), font_name, spawn_extra_text_value);
                        spawn_extra_text_index++;
                    }
                }
            }

            if (spawn_filter_enabled == true)
            {
                if (map_spawn.type != EVERQUEST_SPAWN_INFO_TYPE_CORPSE)
                {
                    glColor3ub(0, 255, 255);

                    draw_arrow_by_heading(spawn_map_x, spawn_map_y, map_spawn.heading, arrow_by_heading_size, false);
                }
            }

            if (spawn_line_enabled == true)
            {
                if (spawn_line_name_data.size())
                {
                    bool found_spawn = false;

                    foreach(std::string spawn_line_name_data_value, spawn_line_name_data)
                    {
                        std::size_t found = map_spawn.name.find(spawn_line_name_data_value);

                        if (found != std::string::npos)
                        {
                            found_spawn = true;
                        }
                    }

                    if (found_spawn == true)
                    {
                        glColor3ub(0.0, 255, 255);

                        draw_line(player_map_x, player_map_y, spawn_map_x, spawn_map_y);
                    }
                }
            }

            if (map_spawn.is_target == true)
            {
                if (map_draw_target_line == true)
                {
                    glColor3ub(0, 255, 0);

                    draw_line(player_map_x, player_map_y, spawn_map_x, spawn_map_y);

                    if (map_draw_target_arrow == true)
                    {
                        draw_arrow_by_heading(spawn_map_x, spawn_map_y, map_spawn.heading, arrow_by_heading_size, true);
                    }
                }
            }

            num_spawns++;
        }

        num_spawns_ex = num_spawns;
    }

    glColor3ub(255, 0, 255);

    draw_plus(player_map_x, player_map_y, 4);

    if (map_draw_player_arrow == true)
    {
        draw_arrow_by_heading(player_map_x, player_map_y, player_heading, arrow_by_heading_size, true);
    }

    if (map_draw_player_name == true)
    {
        draw_bitmap_string(player_map_x, player_map_y + (font_size * 1.5), font_name, "Player");
    }

    draw_time_stop = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    draw_time = draw_time_stop - draw_time_start;

    if (draw_time < 1.0)
    {
        draw_time_count++;
    }
    else
    {
        draw_fps = boost::math::round(draw_time_count / draw_time);

        draw_time_count = 0;
    }

    if (map_draw_info_text == true)
    {
        std::vector<std::string> map_info_text;

        std::stringstream map_info_text_buffer;

        map_info_text_buffer.precision(2);

        map_info_text_buffer << std::fixed;

        if (map_draw_fps == true)
        {
            map_info_text_buffer << "FPS: " << draw_fps;
            map_info_text.push_back(map_info_text_buffer.str());
            map_info_text_buffer.str("");
        }

        map_info_text_buffer << "Window: ";
        map_info_text_buffer << window_width << "x" << window_height;
        map_info_text_buffer << " ";
        map_info_text_buffer << glutGet(GLUT_WINDOW_X) << "," << glutGet(GLUT_WINDOW_Y);
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        map_info_text_buffer << "Zoom: " << map_zoom;
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        map_info_text_buffer << "Offset X: " << map_offset_x;
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        map_info_text_buffer << "Offset Y: " << map_offset_y;
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        if (map_max_z_positive > 0)
        {
            map_info_text_buffer << "Z Positive: " << map_max_z_positive;
            map_info_text.push_back(map_info_text_buffer.str());
            map_info_text_buffer.str("");
        }

        if (map_max_z_negative > 0)
        {
            map_info_text_buffer << "Z Negative: " << map_max_z_negative;
            map_info_text.push_back(map_info_text_buffer.str());
            map_info_text_buffer.str("");
        }

        map_info_text_buffer << "Lines: ";
        if (map_draw_lines == true)
        {
            map_info_text_buffer << num_lines_ex;
        }
        else
        {
            map_info_text_buffer << get_bool_string(map_draw_lines);
        }
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        map_info_text_buffer << "Points: ";
        if (map_draw_points == true)
        {
            map_info_text_buffer << num_points_ex;
        }
        else
        {
            map_info_text_buffer << get_bool_string(map_draw_points);
        }
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        map_info_text_buffer << "Spawns: ";
        if (map_draw_spawns == true)
        {
            map_info_text_buffer << num_spawns_ex;
        }
        else
        {
            map_info_text_buffer << get_bool_string(map_draw_spawns);
        }
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        map_info_text_buffer << "Players: " << get_bool_string(map_draw_spawn_type_player);
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        map_info_text_buffer << "NPCs: " << get_bool_string(map_draw_spawn_type_npc);
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        map_info_text_buffer << "Corpses: " << get_bool_string(map_draw_spawn_type_corpse);
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

/*
        map_info_text_buffer << "Window Always On Top: " << get_bool_string(window_always_on_top);
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");
*/

/*
        map_info_text_buffer << "Center On Target: " << get_bool_string(map_center_on_target);
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");
*/

/*
        map_info_text_buffer << "Spawn Filter: " << get_bool_string(spawn_filter_enabled);
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");
*/

        if (spawn_filter_enabled == true)
        {
            map_info_text_buffer << "Spawn Filter Name: " << spawn_filter_name;
            map_info_text.push_back(map_info_text_buffer.str());
            map_info_text_buffer.str("");
        }

/*
        map_info_text_buffer << "Spawn Line: " << get_bool_string(spawn_line_enabled);
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");
*/

        if (spawn_line_enabled == true)
        {
            map_info_text_buffer << "Spawn Line Name: " << spawn_line_name;
            map_info_text.push_back(map_info_text_buffer.str());
            map_info_text_buffer.str("");
        }

        if (map_max_spawn_distance > 0)
        {
            map_info_text_buffer << "Spawn Distance: " << map_max_spawn_distance;
            map_info_text.push_back(map_info_text_buffer.str());
            map_info_text_buffer.str("");
        }

        if (map_max_spawn_distance_z > 0)
        {
            map_info_text_buffer << "Spawn Z Distance: " << map_max_spawn_distance_z;
            map_info_text.push_back(map_info_text_buffer.str());
            map_info_text_buffer.str("");
        }

        player_y = reverse_sign(player_y);
        player_x = reverse_sign(player_x);

        map_info_text_buffer << "Loc: " << player_y << ", " << player_x << ", " << player_z;
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        player_heading = boost::math::round(player_heading);

        map_info_text_buffer << "Heading: " << player_heading;
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        map_info_text_buffer << "Speed: " << player_movement_speed;
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        int map_info_text_index = 1;
        foreach (std::string map_info_text_value, map_info_text)
        {
            draw_bitmap_string(0, 0 + (font_size * map_info_text_index), font_name, map_info_text_value);
            map_info_text_index++;
        }
    }

    glutSwapBuffers();
}

void init()
{
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glClearColor(0, 0, 0, 0);

    glViewport(0, 0, window_width, window_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, window_width, window_height, 0, 0, 1);
}

int main(int argc, char** argv)
{
    memory.enable_debug_privileges();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition
    (
        (glutGet(GLUT_SCREEN_WIDTH)  - window_width)  / 2,
        (glutGet(GLUT_SCREEN_HEIGHT) - window_height) / 2
    );

    window_id = glutCreateWindow(application_name.c_str());

    window_hwnd = FindWindow("FREEGLUT", application_name.c_str());

    menu_create();

    parse_ini(ini_file);

    glutDisplayFunc(render);
    glutIdleFunc(render);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(hotkeys);
    glutMouseFunc(mouse);
    glutMouseWheelFunc(mouse_wheel);
    glutMotionFunc(motion);
    glutMenuStatusFunc(menu_status);

    glutTimerFunc(100,  update_process, 0);
    glutTimerFunc(1000, update_zone,    0);
    glutTimerFunc(100,  update_spawns,  0);

    update_process(0);
    update_zone(0);
    update_spawns(0);

    parse_ini(map_zone_ini_file.str());

    if (window_start_maximized == true)
    {
        window_always_on_top = false;
        set_window_always_on_top(window_hwnd, window_always_on_top);

        ShowWindow(window_hwnd, SW_MAXIMIZE);
    }

    init();

    glutMainLoop();

    return 0;
}