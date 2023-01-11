local libcamera = package.loadlib(".\\Lua\\plugins\\camera.dll", "luaopen_camera")

local openview = true
local camera = { x = 0, y = 0, z = 0 }
local temp_camera = { x = 0, y = 0, z = 0 }
local locking = false

function on_init()
    libcamera()
end

--图形绘制代码放这里
function on_imgui()
    local camera_x, camera_y, camera_z, locking = GetCameraData()
    if not locking then
        camera = {
            x = camera_x,
            y = camera_y,
            z = camera_z
        }
    end
    if openview then
        ImGui.SetNextWindowBgAlpha(0.35)
        ImGui.SetNextWindowSize(500, 800)
        openview, shouldDraw = ImGui.Begin('camera', openview)

        locking = ImGui.Checkbox("锁定相机坐标", locking)

        SetCameraData(locking, temp_camera.x,temp_camera.y,temp_camera.z)

        temp_camera.x = ImGui.InputFloat("x", camera.x)
        temp_camera.y = ImGui.InputFloat("y", camera.y)
        temp_camera.z = ImGui.InputFloat("z", camera.z)

        ImGui.End()
    end
end