# Mod Settings

[![](https://byob.yarr.is/jackhumbert/mod_settings/cp_version)](https://github.com/jackhumbert/mod_settings/actions/workflows/build.yaml)

![Wt2Z4fgfQT](https://user-images.githubusercontent.com/141431/223482953-f7860657-62dd-4cc9-90ea-e099edfa1691.jpg)

## Installation

[Get the latest release here](https://github.com/jackhumbert/mod_settings/releases) - `packed-v*.zip` in the release contains all of the requirements listed below at their most up-to-date versions (at the time of release). Simply extract it and copy the contents in your game's installation folder.

If you want to install the mod outside of a release (not recommended), the `build/` folder in the repo contains all of the mod-specific files that you can drag into your game's installation folder.

## Configuration

Configuration is done through redscript classes. [See one example here](https://github.com/jackhumbert/in_world_navigation/blob/main/src/redscript/in_world_navigation/InWorldNavigation.reds). Variable & class names are limited to 1024 characters.

## Keybinding Configuration

Specify an .xml file to be processed by Input Loader like, defining a `overridableUI` attribute in a button inside of a mapping:

```xml
<?xml version="1.0"?>
<bindings>
    <context name="VehicleDriveBase" append="true">
        <action name="TestModAction" map="TestModMap"/>
    </context>
    <mapping name="TestModMap" type="Button" >
        <button id="IK_A" overridableUI="MyModTestButton" />
    </mapping>
</bindings>
```

On the redscript side, define a variable using the `overridableUI` value as the name in a class (any class) for your mod like this:

```swift
@runtimeProperty("ModSettings.mod", "Test mod")
public let MyModTestButton: EInputKey = EInputKey.IK_A;
```

You can then listen for your action when the context you specified in the .xml is active:

```swift
player.RegisterInputListener(this, n"TestModAction");
```

And respond to it with a callback: 

```swift
protected cb func OnAction(action: ListenerAction, consumer: ListenerActionConsumer) -> Bool { 
  if Equals(ListenerAction.GetName(action), n"TestModAction") {
    ...
  }
}
```

## Advanced Configuration for Enums

```swift
enum ModSetting {
  OptionA = 0,
  OptionB = 1,
  OptionC = 2
}

class ModSettings {
  @runtimeProperty("ModSettings.mod", "Mod")
  @runtimeProperty("ModSettings.displayName", "UI-ModSetting-Label")
  @runtimeProperty("ModSettings.displayValues.OptionA", "UI-ModSetting-OptionA")
  @runtimeProperty("ModSettings.displayValues.OptionB", "Fixed Option B")
  public let setting: ModSetting = ModSetting.OptionA;
}
```

This example will produce the following results for enum values:

| Enum Value | Display Value |
| --- | --- |
| `OptionA` | Localized text `GetLocalizedText("UI-ModSetting-OptionA")`  |
| `OptionB` | Fixed text `"Fixed Option B"`  |
| `OptionC` | Value name `"OptionC"`  |


## Requirements

* [RED4ext](https://github.com/WopsS/RED4ext)
* [Redscript](https://github.com/jac3km4/redscript)

_For the mod to work with REDmod deployments, the following mod is required:_
* [cybercmd](https://github.com/jac3km4/cybercmd)

## Development

Running `tools/ModStngs.1sc` on `mod_settings_main.inkwidget` will toggle between using a custom class (`ModStngs` replacing `Settings` in `SettingsMainGameController` and `SettingsSelectorController*`) so the file can be opened & edited in Wolvenkit. If you keep the file open in Wolvenkit, you won't need to convert back, and only run the script after you've saved it in Wolvenkit, before packing.

## Bugs

If you come across something that doesn't work quite right, or interferes with another mod, [search for or create an issue!](https://github.com/jackhumbert/mod_settings/issues) I have a lot of things on a private TODO list still, but can start to move things to Github issues.

Special thanks to @psiberx for [Codeware Lib](https://github.com/psiberx/cp2077-codeware/), [InkPlayground Demo](https://github.com/psiberx/cp2077-playground), and Redscript & CET examples on Discord, @WopsS for [RED4ext](https://github.com/WopsS/RED4ext), @jac3km4 for [Redscript toolkit](https://github.com/jac3km4/redscript), @yamashi for [CET](https://github.com/yamashi/CyberEngineTweaks), @rfuzzo & team (especially @seberoth!) for [WolvenKit](https://github.com/WolvenKit/WolvenKit), and all of them for being helpful on Discord.
