const form = document.getElementById("uploadForm");
const statusText = document.getElementById("statusText");
const fileInfo = document.getElementById("fileInfo");
const fileInput = document.getElementById("firmwareFile");
const verifyBtn = document.getElementById("verifyBtn");
const uploadBtn = document.getElementById("uploadBtn");
const verifyBox = document.getElementById("verifyBox");
const fields = ["v", "s", "c", "sig", "bt", "meta"];
const MAX_FILE_SIZE = 2 * 1024 * 1024; // 2 MB

// Khởi tạo: nút Submit bị vô hiệu hóa nếu chưa chọn file
uploadBtn.disabled = true;
loadVersionTree();
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
    statusText.textContent = "";
    verifyBtn.disabled = true;

    const file = fileInput.files[0];
    if (!file) {
        statusText.textContent = "Vui lòng chọn file để upload!";
        verifyBtn.disabled = false;
        return;
    }

    // ❌ CHECK SIZE
    if (file.size > MAX_FILE_SIZE) {
        statusText.textContent = "File vượt quá dung lượng cho phép (tối đa 2 MB)";
        verifyBtn.disabled = false;
        uploadBtn.style.display = "none";
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

    await new Promise(r => setTimeout(r, 500));

    // ----==== ĐỌC PREFIX LENGTH (2 bytes) ====----
    const prefixBuf = await file.slice(0, 2).arrayBuffer();
    const prefixArr = new Uint8Array(prefixBuf);
    const metaLen = prefixArr[0] | (prefixArr[1] << 8);

    // ----==== ĐỌC METADATA ====----
    const metaBuf = await file.slice(2, 2 + metaLen).arrayBuffer();
    const metaStr = new TextDecoder().decode(new Uint8Array(metaBuf));

    const info = {};
    metaStr.split(";").forEach(p => {
        if (p.includes("=")) {
            const [k, v] = p.split("=");
            info[k.trim()] = v.trim();
        }
    });

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

    const allOK = info.VERSION && info.SIZE && info.CRC32 && info.SIGLEN && info.BUILDTIME;

    const metaLine = document.getElementById("meta");
    metaLine.classList.add("show");
    await new Promise(r => setTimeout(r, 300));
    metaLine.querySelector(".state").innerHTML = allOK ? "OK ✅" : "Undefined ❌";

    verifyBtn.disabled = false;

    if (allOK) {
        uploadBtn.style.display = "inline-block";
        uploadBtn.classList.add("fade-in");
        uploadBtn.disabled = false;
    }
};


form.addEventListener("submit", async (e) => {
    e.preventDefault();

    const file = fileInput.files[0];

    if (!file) {
        statusText.textContent = "Vui lòng chọn file để upload!";
        return;
    }

    // ❌ CHECK SIZE
    if (file.size > MAX_FILE_SIZE) {
        statusText.textContent = "File vượt quá dung lượng cho phép (tối đa 2 MB)";
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
        const res = await fetch(
            "/Cubik_8x8x8/Source_Code/Web/Cubik_8x8x8_Admin/Back_end/Upload.php",
            {
                method: "POST",
                body: formData
            }
        );

        if (res.ok) {
            const data = await res.json();
            if (data.status === "ok") {
                statusText.textContent = "Upload thành công!";
                loadVersionTree();
            } else {
                statusText.textContent = data.message;
            }
        } else {
            statusText.textContent = "Upload thất bại.";
        }
    } catch (err) {
        statusText.textContent = "Lỗi kết nối đến server!";
    } finally {
        uploadBtn.disabled = false;
        uploadBtn.textContent = "Upload Firmware";
    }
});

async function loadVersionTree() {
    const tree = document.getElementById("versionTree");
    tree.innerHTML = "<li>Loading...</li>";

    try {
        const res = await fetch(
            "/Cubik_8x8x8/Source_Code/Web/Cubik_8x8x8_Admin/Back_end/ProductCycle.php"
        );

        const json = await res.json();
        if (json.status !== "ok") throw new Error("API error");

        tree.innerHTML = "";
        $("#latestVersion").html(json.latest.version);
        json.data.forEach(item => {
            const li = document.createElement("li");

            // 📁 Version folder
            const folder = document.createElement("span");
            folder.className = "folder";
            folder.textContent = `📁 ${item.version}`;
            li.appendChild(folder);

            const ul = document.createElement("ul");
            ul.className = "subtree";

            // 📄 Firmware
            ul.innerHTML += `
                <li class="file">
                    <a href="${item.file_url}" download>
                        ${item.file_name}
                    </a> (${item.size_kb} KB)
                </li>
                <li class="file">Upload date: ${item.uploaded_at}</li>
            `;

            // Metadata
            Object.entries(item.metadata).forEach(([k, v]) => {
                ul.innerHTML += `<li class="file">${k}: ${v}</li>`;
            });

            // Description
            if (item.description) {
                ul.innerHTML += `<li class="file">Description: ${item.description}</li>`;
            }

            li.appendChild(ul);
            tree.appendChild(li);

            folder.onclick = () => {
                const open = ul.classList.toggle("show");
                folder.textContent = open
                    ? `📂 ${item.version}`
                    : `📁 ${item.version}`;
            };
        });

    } catch (err) {
        tree.innerHTML = `<li class="error">${err.message}</li>`;
    }
}


// Load khi mở trang
window.onload = () => {
    loadFakeVersionTree();
};
