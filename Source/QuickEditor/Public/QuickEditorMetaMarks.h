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

		// specify if item will create a window 
        // [function type]:			static 
        // [function signature]:	void(void)
		// [meta value]:			window type, "[1,2,3...] | [Win/Dock/Modal]", token one mains max window num, default is 1
		// [meta value]:			eg. "1|Dock" mains create a single dock window, "Win" mains create a single normal window  
		// [meta value]:			"Modal" mains create a modal window, notice that modal window can only be single window  
		// [!!!InProgress]:			this functional is in develop, don't use it 
		QEWindow ,
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
        // [meta value]:			Asset Category and name, default is Misc, eg. "MyCate::MyAsset", "MyAsset" mains "Misc::MyAsset"
        // [Tips]:					Built in category can be found in EAssetTypeCategories
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

		// custom class detail
		// [function type]:			static
		// [function signature]:	void(void)
		// [meta value]:			None, it just a mark
		// [!!!InProgress]:			this functional is in develop, don't use it 
		QEDetail ,

		// custom property detail 
        // [function type]:			static
        // [function signature]:	void(void)
        // [meta value]:			None, it just a mark
        // [!!!InProgress]:			this functional is in develop, don't use it 
		QEItemDetail ,

		// call in editor, can with params
		// [function type]:			member
        // [function signature]:	void(...)
		// [meta value]:			None, it just a mark
		// [!!!InProgress]:			this functional is in develop, don't use it 		
		QECallInEditor ,
	};

	// meta data for UCLASS 
	enum QE_UC
	{
		// reroute actions to target class
		// [support]:		QEAssetItem, QEActorItem,QECreateNew, QECreateFile, QEDetail 
		// [meta value]:	target class, without prefix, eg. "Actor"
		QEReroute ,

		// may add
	};

	// meta data for UPROPERTY
	enum QE_UP
	{
		// may add
		// use quick editor array layout 
		QEArray ,
		// use quick editor map layout 
		QEMap ,
	};
	
	
}

