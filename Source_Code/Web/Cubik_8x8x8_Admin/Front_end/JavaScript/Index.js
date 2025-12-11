const form = document.getElementById("uploadForm");
const btn = document.getElementById("uploadBtn");
const statusText = document.getElementById("statusText");

form.addEventListener("submit", async (e) => {
    e.preventDefault();

    btn.disabled = true;
    btn.textContent = "Đang upload...";
    statusText.textContent = "";

    const formData = new FormData(form);

    try {
        const res = await fetch("/api/upload_firmware", {
            method: "POST",
            body: formData
        });

        if (res.ok) {
            statusText.textContent = "Upload thành công!";
        } else {
            statusText.textContent = "Upload thất bại.";
        }
    } catch (err) {
        statusText.textContent = "Lỗi kết nối đến server!";
    }

    btn.disabled = false;
    btn.textContent = "Upload Firmware";
})

$("#btn-logout").click(function (event) {
    $.post("/Cubik_8x8x8/Source_Code/Web/Cubik_8x8x8_Admin/Back_end/Logout.php",function(data){})
})