# Refactor Instructions: L-System 3D Renderer

## 1. Project-Wide Standards
- **Namespace:** Wrap all project code in `namespace LSystem`.
- **Logic:** Use early returns to keep the "happy path" at the primary indentation level.
- **Style:** K&R (opening braces on the same line).
- **Efficiency:** Replace stack-allocated arrays like `translations[10000]` with `std::vector<glm::vec3>`.

---

## 2. File: Geometry.hpp / .cpp
**Purpose:** Pure mathematical vertex generation. Removes vertex math from the rendering loop.

- **Methods:**
    - `std::vector<float> generateCylinder(int numSegments, float height, float radius)`
- **Internal Helpers:** Use an anonymous namespace for `generateCirclePoints` to hide it from the linker.
- **Logic Rule:** ```cpp
    if (numSegments < 3) {
        return {}; // Cannot form a cylinder
    }
    ```

---

## 3. File: Shader.hpp / .cpp
**Purpose:** Encapsulate GLSL management.

- **Members:** `GLuint ID` (the Program ID).
- **Methods:**
    - `void load(const std::string& vertexPath, const std::string& fragmentPath)`
    - `void use()`: Wraps `glUseProgram(ID)`.
    - `void setMat4(const std::string& name, const glm::mat4& matrix)`: Uses `glGetUniformLocation`.
- **Logic Rule:** Check for compilation success; if it fails, log and `return`.

---

## 4. File: Camera.hpp / .cpp
**Purpose:** Decouple view math from the main loop.

- **Members:** `pos`, `front`, `up`, `yaw`, `pitch`, `zoom`, `lastX/Y`.
- **Methods:**
    - `glm::mat4 getViewMatrix()`: Returns `glm::lookAt`.
    - `glm::mat4 getProjectionMatrix(float aspect)`: Returns `glm::perspective`.
    - `void processKeyboard(unsigned char key, float deltaTime)`
    - `void processMouse(float xOffset, float yOffset)`
- **Logic Rule:** ```cpp
    void updateCameraVectors() {
        if (pitch > 89.0f) { pitch = 89.0f; }
        // ... calculation logic ...
    }
    ```

---

## 5. File: Renderer.hpp / .cpp
**Purpose:** The hardware abstraction layer. Owns the OpenGL state.

- **Members:** `GLuint vao, vbo, instancesVbo`.
- **Methods:**
    - `void setupMesh(const std::vector<float>& meshData)`: Generates and binds the static cylinder VAO.
    - `void updateInstances(const std::vector<glm::vec3>& positions)`: Uses `glBufferData` or `glBufferSubData` to upload the turtle's results to the `instancesVbo`.
    - `void draw(Shader& shader, Camera& camera, int width, int height)`: Binds VAO, sets camera uniforms via the shader, and calls `glDrawArraysInstanced`.
- **Logic Rule:** ```cpp
    void draw(...) {
        if (vao == 0) {
            return;
        }
        // ... draw call logic ...
    }
    ```

---

## 6. File: main.cpp
**Purpose:** System orchestration and FreeGLUT callback routing.

- **Structure:**
    1. Instantiate `LSystem::Camera`, `LSystem::Shader`, and `LSystem::Renderer`.
    2. Define global "bridge" functions to handle GLUT callbacks (since GLUT requires C-style function pointers).
    3. Initialize the pipeline:
       - `turtle.generate(instructionJson)` -> returns `points`.
       - `renderer.setupMesh(geometry.generateCylinder(...))`.
       - `renderer.updateInstances(points)`.
- **Logic Flow:**
    ```cpp
    void display() {
        if (!appRunning) {
            return;
        }
        renderer->draw(*shader, *camera, windowWidth, windowHeight);
    }
    ```

---

## 7. Build Guidance (CLI context)
When compiling, ensure all `.cpp` files are linked. 
**Example Command:**
`g++ -o lsystem main.cpp Camera.cpp Shader.cpp Renderer.cpp Geometry.cpp -lGLEW -lGL -lglut -lGLU`
