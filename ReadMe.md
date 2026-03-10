# MG Spatial Selection Plugin

[![Plugin version number](https://img.shields.io/github/v/release/cem-akkaya/MGSpatialSelection?label=Version)](https://github.com/cem-akkaya/MGSpatialSelection/releases/latest)
[![Unreal Engine Supported Versions](https://img.shields.io/badge/Unreal_Engine-5.7-9455CE?logo=unrealengine)](https://github.com/cem-akkaya/MGSpatialSelection/releases)
[![License](https://img.shields.io/github/license/cem-akkaya/MGSpatialSelection)](LICENSE)
[![Actively Maintained](https://img.shields.io/badge/Maintenance%20Level-Actively%20Maintained-green.svg)](https://gist.github.com/cheerfulstoic/d107229326a01ff0f333a1d3476e068d)

<img src="https://cemakkaya.com/ImageHost/splash.jpg" alt="plugin-spatial-selection" width="100%"/>


## Overview

MG Spatial Selection is an Unreal Engine plugin designed for high-performance, real-time spatial selection of actors within a volume. It uses a native box-based collision system to detect overlaps efficiently, making it ideal for strategy games, editor tools, or any application requiring precise volume selection.

The component manages a dynamic selection actor that scales and positions itself based on user input, provides real-time feedback through debug visuals, and generates a grid of surface points for environmental interaction.

Integration is straightforward: attach the component to your character or player controller, define your collision channels, and implement the selection interface on any actor you wish to detect.

A demo project showcasing the selection system in action can be found below.  
[MG Spatial Selection Demo UE5 Project](https://github.com/cem-akkaya/MGSpatialSelectionDemo)

If you have any bug or crash, please open an issue in the GitHub repo.  
If you have suggestions, questions or need help, you can always contact [me](https://github.com/cem-akkaya)

If you want to contribute, feel free to create a pull request.

---

## Features

- **High-Performance Selection**: Uses native `UBoxComponent` for efficient overlap detection.
- **Dynamic Bounds**: Real-time calculation of selection center and extent based on cursor position.
- **Terrain-Aware Visuals**: Driven by a Material Parameter Collection (MPC) and a custom C++ Material Node (`MG Selection Mask`), allowing for performant, pixel-perfect Post-Process effects that follow terrain contours.
- **Enhanced Input Integration**: Ready-to-use with Unreal Engine's Enhanced Input system.
- **Interface-Driven Interaction**: Easily filter selectable actors using the `IMGSpatialSelectionInterface`.
- **Batch Processing**: Smart event broadcasting to prevent redundant updates (avoiding 2x trigger issues).
- **Configurable Settings**: Customizable selection height and collision channels.
- **Debug Visuals**: Integrated debug drawing for the selection box and cursor tracking.

---

## Examples

| <img src="Resources/Demo1.gif" width="370"/> | <img src="Resources/Demo2.gif" width="370"/> |
|:--:|:--:|
| High-performance box selection with real-time overlap detection. | Selection area scaling dynamically with cursor movement. |

| <img src="Resources/Demo3.gif" width="370"/> | <img src="Resources/Demo4.gif" width="370"/> |
|:--:|:--:|
| Post-Process selection mask following terrain contours. | Integration with the MGSpatialSelectionInterface for targeted detection. |

| <img src="Resources/Demo5.gif" width="790"/> |
|:--:|
| Batch-processed selection updates ensuring singular event broadcasts per frame. |

| <img src="Resources/Demo6.gif" width="790"/> |
|:--:|
| Debug visualization mode showing bounds and cursor tracking. |

---

## Installation

<img src="Resources/ss1.jpg" alt="plugin-spatial-selection" width="830"/>

Install it like any other Unreal Engine plugin.

- Download and place the MGSpatialSelection plugin under:  
  `Drive:\YOURPROJECTFOLDER\Plugins\MGSpatialSelection`
- Activate the plugin in your project Plugins window.
- Add the **MGSpatialSelection** component to your Player Controller or Character.
- Assign a **Selection Action** (Enhanced Input) in the component settings.
- Implement the **MGSpatialSelectionInterface** on actors you want to be selectable.

---

### Basic Usage

1. **Component Setup**: Add `UMGSpatialSelectionComponent` to your Actor.
2. **Collision Config**: Set the `CollisionChannels` and `TraceChannel` in the Details panel.
3. **Event Binding**: Bind to `OnActorSelected`, `OnActorDeselected`, or `OnSelectionUpdated` in Blueprints or C++.
4. **Interface**: Add `IMGSpatialSelectionInterface` to your target Actor classes. Logic in `OnSelectionStatus` will trigger when selected/deselected.

---


## FAQ

<details>
<summary><b>How are the surface points calculated?</b></summary>

The component performs a vertical line trace (raycast) for each cell in a 2D grid defined by the `GridDensity` setting. It starts from the top of the selection volume and traces downwards to find the world geometry.

</details>

<details>
<summary><b>Can I change the selection height?</b></summary>

Yes. The `SelectionHeight` property on the `UMGSpatialSelectionComponent` determines the vertical extent of the selection volume. This is useful for selecting units on different elevation levels.

</details>

<details>
<summary><b>Does it support multi-select?</b></summary>

Yes. The system tracks all actors that enter the selection volume and implement the `IMGSpatialSelectionInterface`. It broadcasts the full list via `OnSelectionUpdated`.

</details>

---

## Known Limitations / Tips

- **Grid Density Performance**: High `GridDensity` values (low step size) increase the number of raycasts per frame. Balance density with performance based on your terrain complexity.
- **Interface Requirement**: Actors **must** implement `IMGSpatialSelectionInterface` to be detected by the system.
- **Collision Channels**: Ensure your `TraceChannel` and `CollisionChannels` are correctly configured in the component to match your world geometry and units.

## License

This plugin is under the [MIT License](LICENSE).

MIT License does allow commercial use. You can use, modify, and distribute the software in a commercial product without any restrictions.

However, you must include the original copyright notice and disclaimers.

---

## Support Me

If you like the plugin, you can support my work here:

<a href="https://www.buymeacoffee.com/akkayaceq" target="_blank">
<img src="https://cdn.buymeacoffee.com/buttons/default-yellow.png" alt="Buy Me A Coffee" height="41" width="174">
</a>
