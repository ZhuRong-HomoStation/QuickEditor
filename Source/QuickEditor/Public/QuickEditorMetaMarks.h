#pragma once

namespace UM
{
	// Generic meta data 
	enum QE
	{
		// specify icon for menu or bar
		// [meta value]:	icon path, root path should be plugin name or project name, eg. "Project/Icons/Icon.png"
		// [meta value]:	icon style set, style set and icon name, eg. "EditorStyle::GraphEditor.Enum_16x"
		QEIcon ,

		// specify icon size for menu or bar
        // [meta value]:	icon size, default is 40x40, eg. "40, 40"
        QEIconSize ,

		// specify if item is popup
		// [function type]:			static 
		// [function signature]:	void(void)
		// [meta value]:			None, it just a mark
		QEPopUp ,
	};
	
	// meta data for static UFUNCTION 
	enum QE_UF
	{	
		// add menu item to main menu(Level editor)
		// [function type]:			static 
		// [function signature]:	void(void)
		// [meta value]:			menu path, eg. "Test/SubMenu/Section1.Item" 
		QEMenuItem ,

		// add tool bar item to level editor tool bar
		// [function type]:			static
        // [function signature]:	void(void)
        // [meta value]:			tool bar name, eg. "MyToolBar/SubMenu/Section1.Item" 
		QEToolBar ,
		
		// create new asset support(right click content browser)
		// [function type]:			static
		// [function signature]:	void(void)
		// [meta value]:			create button path, eg. "MyAssets/AssetA"
		QECreateNew ,

		// import new asset support(drag file to content)
		// [function type]:			static
        // [function signature]:	void(void)
        // [meta value]:			specify file extension support, split by ',', eg. "txt,json,xml"
		QECreateFile ,

		// add menu item to asset menu(right click asset icon) 
        // [function type]:			static
        // [function signature]:	void(void)
        // [meta value]:			menu path, eg. "Test/SubMenu/Section1.Item" 
        QEAssetItem ,

        // add menu item to actor menu(right click Actor) 
        // [function type]:			static 
        // [function signature]:	void(void)
        // [meta value]:			menu path, eg. "Test/SubMenu/Section1.Item" 
        QEActorItem ,

		// custom detail
		// [function type]:			static
		// [function signature]:	void(void)
		// [meta value]:			None, it just a mark
		QEDetail ,
	};

	// meta data for UCLASS 
	enum QE_UC
	{
		// reroute actions to target class
		// [support]:		QEAssetItem, QEActorItem,QECreateNew, QECreateFile, QEDetail 
		// [meta value]:	target class, without prefix, eg. "Actor"
		QEReroute ,
	};
}

