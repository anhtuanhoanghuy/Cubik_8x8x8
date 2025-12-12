const form = document.getElementById("uploadForm");
const statusText = document.getElementById("statusText");
const fileInfo = document.getElementById("fileInfo");
const fileInput = document.getElementById("firmwareFile");
const verifyBtn = document.getElementById("verifyBtn");
const uploadBtn = document.getElementById("uploadBtn");
const verifyBox = document.getElementById("verifyBox");
const fields = ["v", "s", "c", "sig", "bt", "meta"];

// Khởi tạo: nút Submit bị vô hiệu hóa nếu chưa chọn file
uploadBtn.disabled = true;

// Khi thay đổi file, cập nhật thông tin
fileInput.addEventListener('change', () => {
    resetAll();
    const file = fileInput.files[0];
    if (file) {
        const fileName = file.name;
        const fileExt = fileName.split('.').pop().toLowerCase();
        if (fileExt !== 'bin') {
            statusText.textContent = "File không hợp lệ! Chỉ được upload file .bin";
            return;
        }

        verifyBtn.disabled = false;

        // Hiển thị tên và kích thước file (MB, 2 chữ số thập phân)
        const sizeKB = Math.round(file.size / 1024);
        fileInfo.textContent = `Kích thước: ${sizeKB} KB`;
    } else {
        verifyBtn.disabled = true;
        fileInfo.textContent = "";
    }
});

// Logout
$("#btn-logout").click(function () {
    window.location.href = "../../Back_end/Logout.php";
});

// ✅ Reset toàn bộ trạng thái
function resetAll() {
    verifyBox.style.display = "none";

    fields.forEach(id => {
        const line = document.getElementById(id);
        line.classList.remove("show");
        line.querySelector(".state").innerHTML = "";
    });

    uploadBtn.style.display = "none";
    uploadBtn.classList.remove("fade-in");

    // ✅ Disable verify khi chưa có file
    verifyBtn.disabled = true;
}



// ✅ VERIFY
verifyBtn.onclick = async () => {
    const file = fileInput.files[0];
    if (!file) {
        statusText.textContent = "Vui lòng chọn file để upload!";
        return;
    }
    // Reset trước khi verify lại
    verifyBox.style.display = "none";
    fields.forEach(id => {
        const line = document.getElementById(id);
        line.classList.remove("show");
        line.querySelector(".state").innerHTML = "";
    });
    uploadBtn.style.display = "none";

    verifyBox.style.display = "block";

    // Loading 0.5s
    await new Promise(r => setTimeout(r, 500));

    // ----==== ĐỌC PREFIX LENGTH (2 bytes) ====----
    const prefixBuf = await file.slice(0, 2).arrayBuffer();
    const prefixArr = new Uint8Array(prefixBuf);
    const metaLen = prefixArr[0] | (prefixArr[1] << 8); // little-endian

    // ----==== ĐỌC ĐÚNG METADATA ====----
    const metaBuf = await file.slice(2, 2 + metaLen).arrayBuffer();
    const metaStr = new TextDecoder().decode(new Uint8Array(metaBuf));

    // Parse key=value
    const info = {};
    metaStr.split(";").forEach(p => {
        if (p.includes("=")) {
            const [k, v] = p.split("=");
            info[k.trim()] = v.trim();
        }
    });

    // ----==== Check từng dòng ====----
    const checks = [
        ["v", info.VERSION],
        ["s", info.SIZE],
        ["c", info.CRC32],
        ["sig", info.SIGLEN],
        ["bt", info.BUILDTIME]
    ];

    for (let i = 0; i < checks.length; i++) {
        const [id, ok] = checks[i];
        const line = document.getElementById(id);
        line.classList.add("show");
        await new Promise(r => setTimeout(r, 1000));
        line.querySelector(".state").innerHTML = ok ? `${ok} ✅` : "Undefined ❌";
    }

    // ----==== Metadata tổng ====----
    const allOK = info.VERSION && info.SIZE && info.CRC32 && info.SIGLEN && info.BUILDTIME;

    const metaLine = document.getElementById("meta");
    metaLine.classList.add("show");
    await new Promise(r => setTimeout(r, 300));
    metaLine.querySelector(".state").innerHTML = allOK ? "OK ✅" : "Undefined ❌";

    // ----==== Nếu OK → cho phép upload ====----
    if (allOK) {
        uploadBtn.style.display = "inline-block";
        uploadBtn.classList.add("fade-in");
        uploadBtn.disabled = false;
    }
};

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

    uploadBtn.disabled = true;
    uploadBtn.textContent = "Đang upload...";
    statusText.textContent = "";

    const formData = new FormData(form);
   
    try {
        const res = await fetch("/Cubik_8x8x8/Source_Code/Web/Cubik_8x8x8_Admin/Back_end/Upload.php", {
            method: "POST",
            body: formData
        });

        if (res.ok) {
            const data = await res.json();
            if (data.status === "ok") {
                statusText.textContent = "Upload thành công!";
            } else {
                statusText.textContent = data.message;
            }
        } else {
            statusText.textContent = "Upload thất bại.";
        }
    } catch (err) {
        statusText.textContent = "Lỗi kết nối đến server!";
        uploadBtn.disabled = false;
    }
    uploadBtn.textContent = "Upload Firmware";
});

// --------------------------------------------
// FAKE DATA
const fakeVersionData = [
    {
        version: "version_1.0.0",
        output: {
            name: "output_0xA1B2.bin",
            sizeKB: 120, // KB
            url: "https://example.com/output_0xA1B2.bin"
        },
        uploaded_at: "2025-01-01 12:00:00",
        metadata: {
            VERSION: "1.0.0",
            BUILDTIME: "2025-01-01 12:00:00"
        },
        description: "Fix bug and improve performance."
    },
    {
        version: "version_1.1.0",
        output: {
            name: "output_0xC3D4.bin",
            sizeKB: 135,
            url: "https://example.com/output_0xC3D4.bin"
        },
        uploaded_at: "2025-02-10 09:30:00",
        metadata: {
            VERSION: "1.1.0",
            BUILDTIME: "2025-02-10 09:30:00"
        },
        description: "Fix bug and improve performance."
    },
    {
        version: "version_2.0.0",
        output: {
            name: "output_0x9F22.bin",
            sizeKB: 150,
            url: "https://example.com/output_0x9F22.bin"
        },
        uploaded_at: "2025-03-20 15:45:00",
        metadata: {
            VERSION: "2.0.0",
            BUILDTIME: "2025-03-20 15:45:00"
        },
        description: "Fix bug and improve performance."
    }
];

function loadFakeVersionTree() {
    const tree = document.getElementById("versionTree");
    tree.innerHTML = "";

    fakeVersionData.forEach(item => {
        const li = document.createElement("li");

        // Folder version (icon mở/đóng)
        const folder = document.createElement("span");
        folder.classList.add("folder");
        folder.innerHTML = `📁 ${item.version}`;
        li.appendChild(folder);

        // Subtree (ẩn/hiện)
        const ul = document.createElement("ul");
        ul.classList.add("subtree");

        // File output
        const fileLi = document.createElement("li");
        fileLi.classList.add("file");
        fileLi.innerHTML = `
            <a href="${item.output.url}" download>
                ${item.output.name}
            </a> (${item.output.sizeKB} KB)
        `;
        ul.appendChild(fileLi);

        // Ngày upload
        const dateLi = document.createElement("li");
        dateLi.classList.add("file");
        dateLi.innerHTML = `Upload date: ${item.uploaded_at}`;
        ul.appendChild(dateLi);

        // Metadata
        for (const [key, value] of Object.entries(item.metadata)) {
            const metaLi = document.createElement("li");
            metaLi.classList.add("file");
            metaLi.innerHTML = `${key}: ${value}`;
            ul.appendChild(metaLi);
        }

        // Description
        const description = document.createElement("li");
        description.classList.add("file");
        description.innerHTML = `Description: ${item.description}`;
        ul.appendChild(description);

        li.appendChild(ul);
        tree.appendChild(li);

        // ✅ Toggle mở/đóng + đổi icon folder + slide-down
        folder.addEventListener("click", () => {
            const isOpen = ul.classList.toggle("show");

            folder.innerHTML = isOpen
                ? `📂 ${item.version}`   // mở
                : `📁 ${item.version}`;  // đóng
        });
    });
}



// Load khi mở trang
window.onload = () => {
    loadFakeVersionTree();
};
