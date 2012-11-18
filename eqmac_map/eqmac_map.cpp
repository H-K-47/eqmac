#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <boost/math/constants/constants.hpp>

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

const std::string ini_file = "eqmac_map.ini";

memory memory;

int window_id = 0;

HWND window_hwnd;

int window_width  = 640;
int window_height = 480;

bool window_always_on_top = false;

const float PI = boost::math::constants::pi<float>();

float map_origin_x = window_width  / 2;
float map_origin_y = window_height / 2;

float MAP_ZOOM_DEFAULT = 1.0;

float MAP_ZOOM_MIN = 0.1;

float map_zoom = MAP_ZOOM_DEFAULT;

float map_zoom_multiplier = 0.25;

float map_offset_x = 0;
float map_offset_y = 0;

float map_offset_multiplier = 10.0;

float map_draw_x = 0;
float map_draw_y = 0;

std::string map_folder = "c:/eqmac/maps";

bool map_draw_info_text = true;
bool map_draw_lines     = true;
bool map_draw_points    = true;
bool map_draw_spawns    = false;

bool map_draw_player_name = false;

std::string map_zone = "qeynos";

void *font_name = GLUT_BITMAP_HELVETICA_10;

int font_size = 10;

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

    int r;
    int g;
    int b;
};

std::vector<map_line_t> map_lines;
std::vector<map_line_t>::iterator map_lines_it;

struct map_point_t
{
    float x;
    float y;
    float z;

    int r;
    int g;
    int b;

    int size;

    std::string text;
};

std::vector<map_point_t> map_points;
std::vector<map_point_t>::iterator map_points_it;

struct map_spawn_t
{
    int address;

    std::string name;

    float x;
    float y;
    float z;

    float heading;

    float movement_speed;

    int standing_state;

    bool is_target;
};

std::vector<map_spawn_t> map_spawns;
std::vector<map_spawn_t>::iterator map_spawns_it;

bool spawn_filter_enabled = false;

std::string spawn_filter_name = "";

#define SPAWNS_MAX 4096

#define MAP_SPAWN_NAMES_MAX_VISIBLE 100

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
        result = value - value * 2;
    }
    else
    {
        result = abs(value);
    }

    return result;
}

void toggle_bool(bool &b)
{
    if (b == true)
    {
        b = false;
    }
    else
    {
        b = true;
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

void draw_plus(float x, float y)
{
    glBegin(GL_LINES);
        glVertex2f(x - 4, y);
        glVertex2f(x + 4, y);

        glVertex2f(x, y - 4);
        glVertex2f(x, y + 4);
    glEnd();
}

void draw_arrow_by_heading(float x, float y, float heading, int size)
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

void map_draw_info_text_toggle()
{
    toggle_bool(map_draw_info_text);
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

void update_process(int value)
{
    HWND everquest_hwnd = FindWindow(NULL, everquest_title.c_str());

    memory.set_process_by_hwnd(everquest_hwnd);

    glutTimerFunc(100, update_process, 0);
}

void update_zone(int value)
{
    glutTimerFunc(1000, update_zone, 0);

    std::string zone_short_name = everquest_get_zone_short_name(memory);

    boost::algorithm::to_lower(zone_short_name);

    std::string zone_long_name = everquest_get_zone_long_name(memory);

    std::stringstream window_title_buffer;
    window_title_buffer << zone_long_name << " " << "(" << zone_short_name << ")" << " - eqmac_map";

    glutSetWindowTitle(window_title_buffer.str().c_str());

    //std::cout << "zone_short_name:" << zone_short_name << std::endl;
    //std::cout << "map_zone:" << map_zone << std::endl;

    if (zone_short_name == map_zone)
    {
        return;
    }

    map_zone.assign(zone_short_name);

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

/*
            foreach (std::string line_data_value, line_data)
            {
                std::cout << "line_data_value: " << line_data_value << std::endl;
            }
*/

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

    map_zoom_reset();
    map_center();
}

void update_spawns(int value)
{
    map_spawns.clear();

    int target_spawn_info = everquest_get_target_spawn_info(memory);

    int player_spawn_info = everquest_get_player_spawn_info(memory);

    //float player_y = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
    //float player_x = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
    //float player_z = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

    int spawn_info_address = player_spawn_info;

    int spawn_next_spawn_info = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER, 4);

    spawn_info_address = spawn_next_spawn_info;

    for (int i = 0; i < SPAWNS_MAX; i++)
    {
        spawn_next_spawn_info = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER, 4);

        if (spawn_next_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
        {
            break;
        }

        map_spawn_t map_spawn;

        map_spawn.address = spawn_info_address;

        map_spawn.name = memory.read_string(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NAME, 0x40);

        map_spawn.y = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Y);
        map_spawn.x = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_X);
        map_spawn.z = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Z);

        map_spawn.heading = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HEADING);

        map_spawn.movement_speed = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_MOVEMENT_SPEED);

        map_spawn.standing_state = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_STANDING_STATE, 1);

        if (spawn_info_address == target_spawn_info)
        {
            map_spawn.is_target = true;
        }
        else
        {
            map_spawn.is_target = false;
        }

        map_spawns.push_back(map_spawn);

        spawn_info_address = spawn_next_spawn_info;
    }

    glutTimerFunc(100, update_spawns, 0);
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27: // Escape
            glutDestroyWindow(window_id);
            exit(0);
            break;

        case 8: // Backspace
            //glutFullScreenToggle();
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
        case 93: // } ]
            map_zoom_in();
            break;

        case 45: // Numpad Subtract or - _
        case 91: // { [
            map_zoom_out();
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
            map_zoom_reset();
            map_center();
            break;

        case GLUT_KEY_F11:
            //glutFullScreenToggle();
            break;

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

        case GLUT_KEY_INSERT:
            map_zoom_in();
            break;

        case GLUT_KEY_DELETE:
            map_zoom_out();
            break;

        case GLUT_KEY_PAGE_UP:
            map_zoom_in();
            break;

        case GLUT_KEY_PAGE_DOWN:
            map_zoom_out();
            break;
    }  
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
    {
        map_zoom_reset();
    }

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
        map_offset_x = map_offset_x + (map_origin_x - x) * map_zoom;
        map_offset_y = map_offset_y + (map_origin_y - y) * map_zoom;
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        if (x != map_origin_x)
        {
            if (x > map_origin_x)
            {
                if (x > map_origin_x + (map_origin_x / 2))
                {
                    map_scroll_right();
                }
            }
            else
            {
                if (x < map_origin_x + (map_origin_x / 2))
                {
                    map_scroll_left();
                }
            }
        }

        if (y != map_origin_y)
        {
            if (y > map_origin_y)
            {
                if (y > map_origin_y + (map_origin_y / 2))
                {
                    map_scroll_down();
                }
            }
            else
            {
                if (y < map_origin_y + (map_origin_y / 2))
                {
                    map_scroll_up();
                }
            }
        }
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
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //glTranslatef(0.375, 0.375, 0);

    int arrow_size = 16;

    int player_spawn_info = everquest_get_player_spawn_info(memory);

    float player_y = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
    float player_x = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
    //float player_z = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

    float player_heading = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING);

    float player_map_x = map_origin_x + (map_offset_x / map_zoom);
    float player_map_y = map_origin_y + (map_offset_y / map_zoom);

    map_draw_x = player_x;
    map_draw_y = player_y;

    if (map_draw_lines == true)
    {
        glBegin(GL_LINES);

        num_lines = 0;

        foreach (map_line_t map_line, map_lines)
        {
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

            draw_plus(point_map_x, point_map_y);

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
            glColor3ub(64, 64, 64);

            float spawn_y = reverse_sign(map_spawn.y);
            float spawn_x = reverse_sign(map_spawn.x);

            float spawn_map_x = ((spawn_x / map_zoom) + map_origin_x) + (player_x / map_zoom) + (map_offset_x / map_zoom);
            float spawn_map_y = ((spawn_y / map_zoom) + map_origin_y) + (player_y / map_zoom) + (map_offset_y / map_zoom);

            float spawn_map_distance = calculate_distance(map_origin_x, map_origin_y, spawn_map_x, spawn_map_y);

            if (map_spawn.is_target != true)
            {
                if (spawn_map_distance > map_origin_x && spawn_map_distance > map_origin_y)
                {
                    continue;
                }
            }

            if (spawn_filter_enabled == true)
            {
                size_t found;
                found = map_spawn.name.find(spawn_filter_name);
                if (found == std::string::npos)
                {
                    continue;
                }
            }

            draw_plus(spawn_map_x, spawn_map_y);

            bool draw_spawn_name = true;

            if (num_spawns_ex > MAP_SPAWN_NAMES_MAX_VISIBLE)
            {
                draw_spawn_name = false;
            }

            if (map_spawn.is_target == true)
            {
                draw_spawn_name = true;
            }

            if (draw_spawn_name == true)
            {
                draw_bitmap_string(spawn_map_x, spawn_map_y + (font_size * 1.5), font_name, map_spawn.name);
            }

            if (map_spawn.is_target == true)
            {
                glColor3f(0.0, 1.0, 0.0);

                draw_line(player_map_x, player_map_y, spawn_map_x, spawn_map_y);

                draw_arrow_by_heading(spawn_map_x, spawn_map_y, map_spawn.heading, arrow_size);
            }

            num_spawns++;
        }

        num_spawns_ex = num_spawns;
    }

    glColor3f(1.0, 0, 1.0);

    if (map_draw_info_text == true)
    {
        std::vector<std::string> map_info_text;

        std::stringstream map_info_text_buffer;

        map_info_text_buffer << "Resolution: " << window_width << "x" << window_height;
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        map_info_text_buffer << "Zoom: " << map_zoom;
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        map_info_text_buffer << "Lines: " << num_lines_ex;
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        map_info_text_buffer << "Points: " << num_points_ex;
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        map_info_text_buffer << "Spawns: " << num_spawns_ex;
        map_info_text.push_back(map_info_text_buffer.str());
        map_info_text_buffer.str("");

        if (spawn_filter_enabled == true)
        {
            map_info_text_buffer << "Spawn Filter Name: " << spawn_filter_name;
            map_info_text.push_back(map_info_text_buffer.str());
            map_info_text_buffer.str("");
        }

        int map_info_text_index = 1;
        foreach (std::string map_info_text_value, map_info_text)
        {
            draw_bitmap_string(0, 0 + (font_size * map_info_text_index), font_name, map_info_text_value);
            map_info_text_index++;
        }
    }

    draw_plus(player_map_x, player_map_y);

    draw_arrow_by_heading(player_map_x, player_map_y, player_heading, arrow_size);

    if (map_draw_player_name == true)
    {
        draw_bitmap_string(player_map_x, player_map_y + (font_size * 1.5), font_name, "Player");
    }

    glutSwapBuffers();
}

void init()
{
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);

    glFrontFace(GL_CW);

    glClearColor(0, 0, 0, 0);

    glViewport(0, 0, window_width, window_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, window_width, window_height, 0, 0, 1);
}

int main(int argc, char** argv)
{
    memory.enable_debug_privileges();

    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_file, pt);

    map_folder = pt.get<std::string>("Options.MapFolder");

    map_draw_info_text = pt.get<bool>("Options.DrawInfoText");
    map_draw_lines     = pt.get<bool>("Options.DrawLines");
    map_draw_points    = pt.get<bool>("Options.DrawPoints");
    map_draw_spawns    = pt.get<bool>("Options.DrawSpawns");

    map_draw_player_name = pt.get<bool>("Options.DrawPlayerName");

    window_width  = pt.get<int>("Window.Width");
    window_height = pt.get<int>("Window.Height");

    window_always_on_top = pt.get<bool>("Window.AlwaysOnTop");

    spawn_filter_enabled = pt.get<bool>("SpawnFilter.Enabled");
    spawn_filter_name    = pt.get<std::string>("SpawnFilter.Name");

    map_origin_x = window_width  / 2;
    map_origin_y = window_height / 2;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition
    (
        (glutGet(GLUT_SCREEN_WIDTH)  - window_width)  / 2,
        (glutGet(GLUT_SCREEN_HEIGHT) - window_height) / 2
    );

    window_id = glutCreateWindow("eqmac_map");

    window_hwnd = FindWindow("FREEGLUT", "eqmac_map");

    if (window_always_on_top == true)
    {
        SetWindowPos(window_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOSIZE);
    }

    glutDisplayFunc(render);
    glutIdleFunc(render);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(hotkeys);
    glutMouseFunc(mouse);
    glutMouseWheelFunc(mouse_wheel);

    glutTimerFunc(100,  update_process, 0);
    glutTimerFunc(1000, update_zone,    0);
    glutTimerFunc(100,  update_spawns,  0);

    update_process(0);
    update_zone(0);

    init();

    glutMainLoop();

    return 0;
}