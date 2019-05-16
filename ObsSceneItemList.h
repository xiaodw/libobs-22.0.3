#pragma once
#include <obs.hpp>
#include <vector>

//维护sceneitem列表数据，及刷新编辑列表
class ObsSceneItemList
{
public:
    ObsSceneItemList();
    ~ObsSceneItemList();

    void Clear() { m_items.clear(); }
    //更新列表
    void SceneChanged();
    void ReorderItems();

    void Add(obs_sceneitem_t *item);
    void Remove(obs_sceneitem_t *item);
    void Select(obs_sceneitem_t *item,bool bSel);

    size_t Count();
    OBSSceneItem Get(int idx);
    OBSSceneItem GetCurrentSceneItem();
private:
    std::vector<OBSSceneItem> m_items;
    int m_select;
};

