# DuetScreen

This firmware provides a graphical user interface to control a Duet3D control board running RRF3.5 or later in standalone mode.

## Upgrading

### Network

> [!NOTE]
> This only works if the screen is connected to the Duet via network.

* Upload the `DuetScreen.bin` file to the `/firmware` directory on the Duet.
* From the screen, navigate to `Settings` -> `Update`

### USB

* Copy the `DuetScreen.bin` file onto a USB stick.
* Insert the USB stick into the screen.
* The screen will prompt you to upgrade the firmware.
* Press the `Upgrade` button to start the upgrade process.
* If you decline the upgrade, the screen will not automatically prompt you again for that firmware version. To upgrade, you must navigate to the `Files` window, select `USB Files` and select the firmware file to upgrade from.

### Fallback

If the firmware becomes corrupted or it is not prompting you to upgrade, rename the file from `DuetScreen.bin` to `upgrade.img` and repeat the progress.
* If this file is found then the screen will continually prompt you to upgrade the firmware even if the upgrade completes successfully.
* Make sure to remove the USB stick once the upgrade is complete to prevent this from happening. 

## Usage

### Changing Communication Mode

### Touch Calibration

### Starting a Guide

### Changing Language

## Developer Documentation

The screen is developed in C++ using the Flythings SDK to provide a GUI framework. All the logic is contained in `src/jni/` with the GUI defined in `src/ui/main.ftu`.

Flythings provides a *What You See Is What You Get* (WYSIWYG) way of creating and modifying the GUI. This requires the use of the Flythings IDE.

Flythings has some documentation on the SDK, it is split between a more up-to-date Chinese version and an older English version. The English version is still useful for understanding the basics of the SDK, but the Chinese version is more up-to-date and has more information.
- https://developer.flythings.cn/zh-hans/system_introdoction.html
- https://developer.flythings.cn/en/system_introdoction.html

### Setup

> [!WARNING]
>
> It is advised to use a virtual machine for development.

* Install Flythings IDE
  * [Environmental Installation · FlyThings Docs](https://developer.flythings.cn/zh-hans/download.html)
* Download the `T113` SDK folder
  * https://download.flythings.cn/sdk/platforms/t113.7z
* Extract the `T113` SDK folder to `C:/flythings/sdk/platforms/`
* Clone/download the `DuetScreen` repository
* Open Flythings IDE
* `Import` Flythings project
  * ![1709737120684](docs/image/README/1709737120684.png)


### Compiling

* ![1709737265559](docs/image/README/1709737265559.png)
  * The green play icon (left) will compile the project.
  * The blue eject icon (right) will compile the project and create an image file that can be used to flash the screen.
    * To convert the `upgrade.img` file to a `DuetScreen.bin` file, simply rename the file.
  * When developing code, it is possible to temporarily flash the screen by using `Ctrl+Alt+R` to compile the firmware and flash it to the screen.
    * You need to configure the ADB configuration in Flythings for this to work
    * ![1709737436213](docs/image/README/1709737436213.png)

#### Inverting Display

This needs to be done at compile time.
* Open the project properties.
* Change the `Screen rotation` setting in the `FlyThings` tab.
  * ![1709747084324](docs/image/README/1709747084324.png)

### Debugging

If you have flythings installed then you can use the IDE to debug the screen.
* https://developer.flythings.cn/en/adb_debug.html

It is possible to debug the screen using `adb` from the command line without having to install the full Flythings IDE.
* Download the android SDK Platform-Tools from the following link:
  * https://developer.android.com/tools/releases/platform-tools
* Add the `platform-tools` folder to your system `PATH`
* Run a terminal and type `adb connect <ip_address_of_screen>`
  * You can find the IP address of the screen by going to `Network` -> `...`
* Start monitoring the logs with `adb logcat`
  * You can add the time with `adb logcat -v time`
  * You can filter by priority with `adb logcat -v time *:E` (error), `adb logcat -v time  *:W` (warning), `adb logcat -v time  *:I` (info), `adb logcat -v time  *:D` (debug), `adb logcat -v time  *:V` (verbose)
  * To you can chain multiple filters together
  * You can also pipe the output to `grep` to filter the output further.

### Modifying GUI

To make fundamental changes to the GUI, you will need to use the Flythings IDE. This lets you move, add, deleted UI elements and set their properties. An overview of what UI elements are available can be found in the [Flythings documentation](https://developer.flythings.cn/en/ctrl_common.html).

> [!IMPORTANT]
> To interact with the UI elements in the code, you need to use the `ID` of the element. Ensure that any element you create has a unique and descriptive `ID` field.

After you have modified and saved the `main.ftu` file with your changes to the GUI, any element that has been added will not be added into the code until you compile the project.
- Flythings will automatically generate the relevant code to interact with the UI elements, this code is placed in the `activity` folder. **DO NOT MODIFY THE CONTENTS OF THE ACTIVITY FOLDER MANUALLY!**.
- Flythings will also add any control functions relevant to the UI element into `src/jni/logic/mainLogic.cc`. Details on these are described in the Flythings documentation.
- Flythings will **NOT** delete any code in `mainLogic.cc` for elements that have been removed from the GUI, you will need to manually remove these.

There are some parts of the UI for this screen that are more complex than a single UI element. Where possible these have be created in a way that allows them to be reused across the GUI. Some examples are the tools list, number pad, slider, popup, and guides.

#### Themes

#### Reusable Components

* Number pad
  * Open with:
    ```cpp
    #include "UI/UserInterface.h"

    UI::NUMPAD_WINDOW->Open(const char* header,
                            const int value,
                            function<void(int)> onValueChanged,
                            function<void(int)> onConfirm);
    ```
  * `onValueChanged` is called when the value changes.
  * `onConfirm` is called when the confirm button is pressed.
  * Can set the position after opening with `SetPosition(UI::HorizontalPosition)` method
  * ![1709737769657](docs/image/README/1709737769657.png)
* Slider
  * Open with:
    ```cpp
    #include "UI/UserInterface.h"

    UI::SLIDER_WINDOW->Open(const char* header, // The header text
				  const char* prefix, // The prefix text, displayed on left of slider
				  const char* suffix, // The suffix text, displayed on right of slider
				  const char* unit,	  // The unit text, displayed after the value
				  const int min,
				  const int max,
				  const int value,
				  function<void(int)> onProgressChanged,
				  bool displayRaw = false // Display the raw value instead of the percentage
		);
    ```
  * Can set the position after opening with `SetPosition(UI::VerticalPosition, UI::HorizontalPosition)` method
  * ![1709738266397](docs/image/README/1709738266397.png)
* Number pad and Slider
  * It is possible to open the number pad and slider together
    ```cpp
    #include "UI/UserInterface.h"
    
    UI::OpenSliderNumPad(const char* header,
                         const char* prefix,
                         const char* suffix,
                         const char* unit,
                         const int min,
                         const int max,
                         const int value,
                         function<void(int)> callback,
                         bool displayRaw = false);
    ```
  * In this case, when the 2 input methods are linked so when the slider is changed, the number pad will also be update, and vice versa.
  * `callback` is called when the value is changed by either entry method.
  * If a number is entered in the number pad that exceeds the min/max boundaries, then `callback` will not be called.
  * ![1709738382968](docs/image/README/1709738382968.png)
* Popup
  * The popup is reused widely across the GUI, some examples include:
    * Full RRF3.5 M291 support
    * Starting prints/macros
    * Displaying new console messages from the Duet
  * ![1709738738659](docs/image/README/1709738738659.png)
  * It can be opened with:
    ```cpp
    #include "UI/UserInterface.h"

    UI::POPUP_WINDOW->Open(function<void(void)> okCb,
                           function<void(void)> cancelCb);
    ```
  * Can set the position after opening with `SetPosition(UI::VerticalPosition, UI::HorizontalPosition)` method
  * Various other helper methods are available in [UI/Popup.h](src/jni/logic/UI/Popup.h)
* Guides
  * Guides are useful for creating walkthroughs for the user. This might be useful for first time setup, or for showing the user how to use a new feature.
  * A guide consists of a number of pages, to give the some flexibility in updating guides without needing a recompile, the contents of a page are shown with an image, this image can be located on external storage (SD card, or USB stick).
  * To create a new guide, you need to create a new `.cpp` file in the `UI/Guides` folder.
  * First create an instance of the `Guide` class, this should define:
    * The guide ID (should be unique), this is used to lookup the guide in the code to start it. It is also used in the guides menu to lookup the human readable guide name to display to the user from the language files.
    * Whether the guide can be closed at any point or if it must be completed.
    * A (optional) window ID, this is the window that will be opened when the guide is started. This is useful if you want to add extra functionality to all the pages in a guide.
  * Next create an array of `Page` objects, each page should define:
    * The guide ID (should be the same as the guide ID defined in the `Guide` object)
    * The path to the image file to display
      * The image will be shown full screen and should preferably be a `.png` file (some other formats are supported)
      * The image should be 1024x600 pixels
    * A (optional) next callback, this is called when the next button is pressed
    * A (optional) previous callback, this is called when the previous button is pressed
    * A (optional) window ID, this is the window that will be opened when the page is shown. This is useful if you want to add extra functionality to a specific page.
    ```cpp
    #include "DebugLevels.h"
    #define DEBUG_LEVEL DEBUG_LEVEL_DBG
    #include "Debug.h"

    #include "UI/GuidedSetup.h"
    #include "UI/UserInterface.h"

    namespace UI::GuidedSetup
    {
        Guide setupGuide("setup", true, ID_MAIN_SetupGuideWindow);

        Page setupPages[] = {Page{
                                "setup",		  // guideId
                                "page1_img.JPG",  // imagePath
                                []() {			  // nextCb
                                    dbg("page1 nextCb");
                                },
                                []() { // previousCb
                                    dbg("page1 previousCb");
                                },
                                NULL // window
                            },
                            Page{
                                "setup",		 // guideId
                                "page2_img.png", // imagePath
                                []() {			 // nextCb
                                    dbg("page2 nextCb");
                                },
                                []() { // previousCb
                                    dbg("page2 previousCb");
                                },
                                ID_MAIN_PageOverlayExampleWindow // window
                            }};

    } // namespace UI::GuidedSetup
    ```
  * ![1709739684424](docs/image/README/1709739684424.png)
* Tools List
  * Because of the flexibility RRF allows in configuring tools, accurately representing the state of all the tools in a useful and complete way can become quite complex if you do not know what tools the user will configure. Also since there are a few pages that it is desirable to show the tools list, it is useful to have a reusable component for this.
  * ![1709739921241](docs/image/README/1709739921241.png)
  * Examples for use can be found in [mainLogic.cc](src/jni/logic/mainLogic.cc)
    ```cpp
	UI::ToolsList::Create("home")->Init(mToolListViewPtr);
    ```

#### Add New Language

* Flythings supports internationalization (i18n) through the use of [language files](src/i18n/).
* The a new file should be created for each language, the file should be named `xx_XX-YYY.tr` and should be placed in the `i18n` folder.
  * `YYY` can be any length and will be the text that is displayed in the language selection menu.
  * https://developer.flythings.cn/zh-hans/i18n.html#%E6%94%AF%E6%8C%81%E6%9B%B4%E5%A4%9A%E7%9A%84%E8%AF%AD%E8%A8%80
  * You need to add the [built-in interface translation text](https://docs.flythings.cn/src/zh_CN.tr) to the custom language, and translate it accordingly, and the built-in interface can display the text normally when switching to the corresponding language;
* The file needs to contain the following for each string that needs to be translated:
  ```xml
  <string name="key">Text to display</string>
  ```
* For static text, the text can be set in Flythings IDE by using the `Text` property of the GUI element and prefixing the key with `@`.
* For dynamic text
  * Store the string format in the language file, eg:
    ```xml
    <string name="babystepping_offset">Current Offset: %.2f mm</string>
    ```
  * Call the following function in the code to set the text:
    ```cpp
    UI::GetUIControl<ZKTextView>(ID_OF_UI_ELEMENT)->setTextTrf("babystepping_offset", 1.23f);
    ```

### Modify Logic

#### Windows / Overlays

* The `UI::WINDOW` object is used to navigate around the GUI. It can be used to open and close windows and overlays.
* `UI::WINDOW` will automatically handle the back button and the home button.
* Window:
  * Only 1 window can be open at a time. Opening a new window will close the current window.
    ```cpp
    #include "UI/UserInterface.h"
    UI::WINDOW->Open(mWindowPtr);
    ```
* Overlay:
  * Multiple overlays can be open at the same time. Opening a new overlay will close the current overlay by default unless specifically requested not to.
    ```cpp
    #include "UI/UserInterface.h"
    UI::WINDOW->OpenOverlay(mOverlayWindowPtr, closeAlreadyOpen);
    ```

#### Accessing GUI Elements

* When you create a GUI element in the `main.ftu` file, it will automatically create a static pointer (`m{id_of_element}Ptr`) to the element in `mainActivity.cpp` and a `#define ID_MAIN_{id_of_element} xxxx` in `mainActivity.h`
* To access the element in the program there are 2 ways to do it:
  * If you are working in a file that gets included in `mainActivity.cpp` (eg. `mainLogic.cc`), you can use the static pointer directly:
    ```cpp
    m<id_of_element>Ptr;
    ```
  * If you are working in any other file, you can retreive the pointer using the `GetUIControl` function:
    ```cpp
    #include "UI/UserInterface.h"

    UI::GetUIControl<ZKTextView>(ID_MAIN_{id_of_element});
    ```
    * Replace `ZKTextView` with the type of the element you are trying to access
  
  #### Using Object Model Information

  * The Duet Object Model (OM) is constantly being requested from the Duet and is used to update the GUI.
  * The screen keeps an internal representation of some important parts of the OM.
  * If there is a part of the OM that you want to use, you need to add an `Observer` to that part of the OM.
    * `Observers` can be found in `src/jni/logic/UI/Observers/`
    * Create a new observer file, or reuse an appropriate existing observer file.
    * The OM is received as a JSON string that is parsed with an internal JSON parser.
    * The OM key is represented by a `String` object with the following format:
      * A `^` character indicates the position of an array index, and a `:` character indicates a field separator.
        * For example, `move.axes[0].homed` would be represented as `move:axes^:homed`
  * There are 2 types of observers:
    * `UI::Observer<UI::ui_field_update_cb>`: This type of observer is used to run a callback when a specific OM field has been received.
      * There are various macros created that will automatically convert the received value into the appropriate type and pass the value to the callback if successful.
    * `UI::Observer<UI::ui_array_end_update_cb>`: This type of observer is used to run a callback when the end of an array has been received.
