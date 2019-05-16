#pragma once
#include <obs.hpp>
#include <vector>

//ά��sceneitem�б����ݣ���ˢ�±༭�б�
class ObsSceneItemList
{
public:
    ObsSceneItemList();
    ~ObsSceneItemList();

    void Clear() { m_items.clear(); }
    //�����б�
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

