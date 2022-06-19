# Renderer and 3D Asset Fileloader

A project to learn more about 3D asset file loading to render 3D models and their animations while also implementing rendering techniques using OpenGL and Vulkan.

## List of Features

|     **Name**     |                                        **Description**                                         |
| :--------------: | :--------------------------------------------------------------------------------------------: |
| FBX File Loading |    Used the Autodesk FBX SDK to load mesh data and load data into internal data structures     |
|  Phong Lighting  |                                      Directional lighting                                      |
|   Shadow maps    |                                      Directional shadows                                       |
|  Shader Editor   |                     Allow users to modify shader texture paths and colors                      |
|   Mesh Editor    | Allow users to change mesh settings such as transforms, display settings, and attached shaders |

### Project Structure

```graphql
./Editor
  ├─ assets - 3D assets and font files used to load into application or render certain fonts
  ├─ ext - external libraries used
  ├─ src - source code
  │  - application entry point, windowing, scene rendering, and helper functions
  │  ├─ images - texture images
  │  ├─ panels - panels used to modify various render and mesh settings
  │  ├─ primitives - basic 3D meshes primarily for debugging transformations of objects
  │  └─ renderer - fundamental renderer objects to display vertex data to the screen
  │  └─ shaders - source code for shaders
./FileLoader
  ├─ ext - external libraries used
  ├─ src - source code for loading fbx files into internal data structures
```
