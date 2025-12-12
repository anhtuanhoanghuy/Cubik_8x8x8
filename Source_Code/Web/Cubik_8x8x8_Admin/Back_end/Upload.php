<?php
require_once 'Database.php';
$conn = db_connect();
header("Content-Type: application/json");

// -----------------------------------------
// 1. Kiểm tra file upload
// -----------------------------------------
if (!isset($_FILES["firmware"])) {
    echo json_encode(["status" => "error", "message" => "No file uploaded"]);
    exit;
}

$tmpPath  = $_FILES["firmware"]["tmp_name"];
$fileName = $_FILES["firmware"]["name"];

// Đọc toàn bộ file
$data = file_get_contents($tmpPath);
if ($data === false) {
    echo json_encode(["status" => "error", "message" => "Cannot read file"]);
    exit;
}

// -----------------------------------------
// 2. Đọc prefix 2 byte → metadata length
// -----------------------------------------
if (strlen($data) < 2) {
    echo json_encode(["status" => "error", "message" => "Invalid firmware"]);
    exit;
}

// unpack("n") = đọc uint16 BE
$metaLen = unpack("n", substr($data, 0, 2))[1];

// Kiểm tra kích thước hợp lệ
if (strlen($data) < 2 + $metaLen) {
    echo json_encode(["status" => "error", "message" => "Corrupted metadata"]);
    exit;
}

// -----------------------------------------
// 3. Lấy metadata string
// -----------------------------------------
$metaRaw = substr($data, 2, $metaLen);
$metaStr = trim($metaRaw, "\0");

// Parse dạng KEY=VALUE;
$parts = explode(";", $metaStr);
$info = [];

foreach ($parts as $p) {
    if (strpos($p, "=") !== false) {
        list($k, $v) = explode("=", $p, 2);
        $info[$k] = $v;
    }
}

// -----------------------------------------
// 4. Validate metadata
// -----------------------------------------
$required = ["VERSION", "SIZE", "CRC32", "SIGLEN", "BUILDTIME"];

foreach ($required as $k) {
    if (!isset($info[$k])) {
        echo json_encode(["status" => "error", "message" => "Missing metadata: $k"]);
        exit;
    }
}

$version = $info["VERSION"];
// -----------------------------------------
// 4.1. Check version trùng DB
// -----------------------------------------
$sql = "SELECT COUNT(*) AS cnt FROM firmware_versions WHERE version = ?";
$stmt = $conn->prepare($sql);
$stmt->execute([$version]);

$row = $stmt->fetch(PDO::FETCH_ASSOC);
$count = (int)$row['cnt'];

if ($count > 0) {
    echo json_encode([
        "status" => "error",
        "message" => "Version $version đã tồn tại trong server. Hãy tăng VERSION trước khi upload."
    ]);
    exit;
}


// -----------------------------------------
// 5. Lưu firmware ra thư mục OTA
// -----------------------------------------
$baseDir = $_SERVER["DOCUMENT_ROOT"] . "/ota/stm32";
$versionDir = "$baseDir/$version";

if (!is_dir($versionDir)) {
    mkdir($versionDir, 0777, true);
}

$firmwarePath = "$versionDir/firmware.bin";

// Move file .bin đã upload
move_uploaded_file($tmpPath, $firmwarePath);

// -----------------------------------------
// 6. Tạo manifest.json
// -----------------------------------------
$manifest = [
    "version"      => $info["VERSION"],
    "size"         => intval($info["SIZE"]),
    "crc32"        => $info["CRC32"],
    "siglen"       => intval($info["SIGLEN"]),
    "build_time"   => $info["BUILDTIME"],
    "firmware_url" => "/ota/esp32/$version/firmware.bin"
];

$manifestPath = "$versionDir/manifest.json";
file_put_contents($manifestPath, json_encode($manifest, JSON_PRETTY_PRINT));

// -----------------------------------------
// 7. Cập nhật latest.json
// -----------------------------------------
$latest = [
    "latest"   => $version,
    "manifest" => "/fota/stm32/$version/manifest.json"
];

file_put_contents("$baseDir/latest.json", json_encode($latest, JSON_PRETTY_PRINT));

// -----------------------------------------
// 8. Ghi vào database
// -----------------------------------------
$sql = "INSERT INTO firmware_versions(
            version, file_path, manifest_path, size, crc32, siglen, build_time, description
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
$stmt = $conn->prepare($sql);
$stmt->execute([$version,
                $firmwarePath,
                $manifestPath,
                $info["SIZE"],
                $info["CRC32"],
                $info["SIGLEN"],
                $info["BUILDTIME"],
                "fix bug"]);
// -----------------------------------------
// 9. Phản hồi cho frontend
// -----------------------------------------
echo json_encode(["status" => "ok", "message" => "Success"]);
exit;

?>
