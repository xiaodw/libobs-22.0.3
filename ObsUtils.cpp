#include "ObsUtils.h"

void EnumGameProcess(enum_game_callback callback, void * ptr)
{
    obs_properties_t* prop = obs_properties_by_id("game_capture");
    if (prop)
    {
        obs_property_t *property = obs_properties_first(prop);

        while (property) {
            const char        *name = obs_property_name(property);
            obs_property_type type = obs_property_get_type(property);
            const char *desc = obs_property_description(property);

            blog(LOG_INFO, "[%s]:%s", name, desc);

            if (strcmp(name, "window") == 0 && type == OBS_PROPERTY_LIST)
            {
                size_t  count = obs_property_list_item_count(property);
                //game capture.c:1952 添加了空字符串在前面，故从1开始
                for (size_t i = 1; i < count; i++)
                {
                    const char *name = obs_property_list_item_name(property, i);
                    const char *window = obs_property_list_item_string(property, i);

                    if(callback)
                        callback(ptr, name, window);
                    blog(LOG_INFO,"GameProcess:%s:%s",name,window);
                }
            }

            obs_property_next(&property);
        }
        obs_properties_destroy(prop);
    }
}


void _enum_game_callback(void* ptr, const char* name, const char* window)
{
    std::vector<GameWindowInfo>* info = (std::vector<GameWindowInfo>*)ptr;
    info->push_back(GameWindowInfo(name,window));
}

std::vector<GameWindowInfo> GetGameWindowInfo()
{
    std::vector<GameWindowInfo> infos;
    EnumGameProcess(_enum_game_callback, &infos);
    return infos;
}

