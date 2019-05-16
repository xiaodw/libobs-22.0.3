#include "ObsSceneItemList.h"
#include "ObsMain.h"

static inline OBSScene GetCurrentScene()
{
    return ObsMain::Instance()->GetCurrentScene();
}

ObsSceneItemList::ObsSceneItemList()
    :m_select(-1)
{

}

ObsSceneItemList::~ObsSceneItemList()
{

}

static bool enumItem(obs_scene_t*, obs_sceneitem_t *item, void *ptr)
{
    std::vector<OBSSceneItem> &items =
        *reinterpret_cast<std::vector<OBSSceneItem>*>(ptr);

    if (obs_sceneitem_is_group(item)) {
        obs_data_t *data = obs_sceneitem_get_private_settings(item);

        bool collapse = obs_data_get_bool(data, "collapsed");
        if (!collapse) {
            obs_scene_t *scene =
                obs_sceneitem_group_get_scene(item);

            obs_scene_enum_items(scene, enumItem, &items);
        }

        obs_data_release(data);
    }
    items.insert(items.begin(), item);
    return true;
}

void ObsSceneItemList::SceneChanged()
{
    OBSScene scene = GetCurrentScene();
    if (scene)
    {
        m_items.clear();
        obs_scene_enum_items(scene, enumItem, &m_items);
    }
}

void ObsSceneItemList::ReorderItems()
{
    OBSScene scene = GetCurrentScene();

    std::vector<OBSSceneItem> newitems;
    obs_scene_enum_items(scene, enumItem, &newitems);

    /* if item list has changed size, do full reset */
    if (newitems.size() != m_items.size()) {
        SceneChanged();
        return;
    }

    for (;;) {
        int idx1Old = 0;
        int idx1New = 0;
        int count;
        int i;

        /* find first starting changed item index */
        for (i = 0; i < newitems.size(); i++) {
            obs_sceneitem_t *oldItem = m_items[i];
            obs_sceneitem_t *newItem = newitems[i];
            if (oldItem != newItem) {
                idx1Old = i;
                break;
            }
        }

        /* if everything is the same, break */
        if (i == newitems.size()) {
            break;
        }

        /* find new starting index */
        for (i = idx1Old + 1; i < newitems.size(); i++) {
            obs_sceneitem_t *oldItem = m_items[idx1Old];
            obs_sceneitem_t *newItem = newitems[i];

            if (oldItem == newItem) {
                idx1New = i;
                break;
            }
        }

        /* if item could not be found, do full reset */
        if (i == newitems.size()) {
            SceneChanged();
            return;
        }

        /* get move count */
        for (count = 1; (idx1New + count) < newitems.size(); count++) {
            int oldIdx = idx1Old + count;
            int newIdx = idx1New + count;

            obs_sceneitem_t *oldItem = m_items[oldIdx];
            obs_sceneitem_t *newItem = newitems[newIdx];

            if (oldItem != newItem) {
                break;
            }
        }
    }
}

void ObsSceneItemList::Add(obs_sceneitem_t *item)
{
    if (obs_sceneitem_is_group(item)) {
        SceneChanged();
    }
    else {
        m_items.insert(m_items.begin(), item);
    }
}
void ObsSceneItemList::Remove(obs_sceneitem_t *item)
{
    int idx = -1;
    for (int i = 0; i < m_items.size(); i++) {
        if (m_items[i] == item) {
            idx = i;
            break;
        }
    }

    if (idx == -1)
        return;

    int startIdx = idx;
    int endIdx = idx;

    bool is_group = obs_sceneitem_is_group(item);
    if (is_group) {
        obs_scene_t *scene = obs_sceneitem_group_get_scene(item);

        for (int i = endIdx + 1; i < m_items.size(); i++) {
            obs_sceneitem_t *subitem = m_items[i];
            obs_scene_t *subscene =
                obs_sceneitem_get_scene(subitem);

            if (subscene == scene)
                endIdx = i;
            else
                break;
        }
    }

    m_items.erase(m_items.begin()+idx, m_items.begin() + endIdx + 1);
}

size_t ObsSceneItemList::Count()
{
    return m_items.size();
}

OBSSceneItem ObsSceneItemList::Get(int idx)
{
    if (idx <0 || idx >= m_items.size())
        return OBSSceneItem();
    return m_items[idx];
}

OBSSceneItem ObsSceneItemList::GetCurrentSceneItem()
{
    if (m_select <0 || m_select >= m_items.size())
        return OBSSceneItem();
    return m_items[m_select];
}

void ObsSceneItemList::Select(obs_sceneitem_t *item, bool bSel)
{
    int index = -1;
    for (int i=0;i<m_items.size();++i)
    {
        if (m_items[i] == item)
        {
            index = i;
            break;
        }
    }

    if (index > 0 && bSel)
        m_select = index;
    else
        m_select = -1;
}
