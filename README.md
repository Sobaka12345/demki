# demki
Cross-platform Vulkan/OpenGL renderer


The objective of this project was an in-depth exploration of VULKAN, driven by a core aim of mastering the fundamental abstractions within the API. The primary focus was on conceptualizing optimal approaches to interface with these abstractions. To realize this objective, the decision was made to construct a renderer featuring a unified interface for engaging with Graphics APIs. The deliberate design allows for interchangeability with other APIs like OpenGL, a move envisioned to facilitate the establishment of robust abstractions for seamless API interaction.

## Build
To configure the project with a default configuration simply run confuguration script:

```
### Prerequisites
### Python 3 that supports newest conan versions
### Vulkan drivers for your platform
### CMAKE, C++20 building tools

RUN in console

bash confugure_linux_default.sh Release (Linux)
OR
configure_windows_default.bat Debug (WIN)

After generation, CMake presets will be available.
```

## Examples

### Tetris
Inaccurate simple Tetris clone <br><br>
<img src=repo_demos/Tetris.gif width=50%>
### Particles
Particles demonstration from vulkan-tutorial.com<br><br>
<img src=repo_demos/Particles.gif width=50%>
### Cubic
Cube-placing sandbox with 1st person view<br><br>
<img src=repo_demos/Cubic.png width=50%>
### Dummy
Model loading example