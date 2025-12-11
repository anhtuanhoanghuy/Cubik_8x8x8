const form = document.getElementById("uploadForm");
const btn = document.getElementById("uploadBtn");
const statusText = document.getElementById("statusText");
const fileInput = form.querySelector('input[name="firmware"]');
const fileInfo = document.getElementById("fileInfo");

// Khởi tạo: nút Submit bị vô hiệu hóa nếu chưa chọn file
btn.disabled = true;

// Khi thay đổi file, cập nhật thông tin
fileInput.addEventListener('change', () => {
    const file = fileInput.files[0];

    if (file) {
        btn.disabled = false;

        // Hiển thị tên và kích thước file (MB, 2 chữ số thập phân)
        const sizeKB = Math.round(file.size / 1024);
        fileInfo.textContent = `Kích thước: ${sizeKB} KB`;
    } else {
        btn.disabled = true;
        fileInfo.textContent = "";
    }
});

form.addEventListener("submit", async (e) => {
    e.preventDefault();

    const file = fileInput.files[0];

    // Kiểm tra file
    if (!file) {
        statusText.textContent = "Vui lòng chọn file để upload!";
        return;
    }

    const fileName = file.name;
    const fileExt = fileName.split('.').pop().toLowerCase();
    if (fileExt !== 'bin') {
        statusText.textContent = "File không hợp lệ! Chỉ được upload file .bin";
        return;
    }

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
});

// Logout
$("#btn-logout").click(function () {
    window.location.href = "../../Back_end/Logout.php";
});
