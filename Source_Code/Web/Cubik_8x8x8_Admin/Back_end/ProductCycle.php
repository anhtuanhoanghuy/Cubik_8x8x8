<?php
require_once "Database.php";
$conn = db_connect();

header("Content-Type: application/json");

// -------------------------------------
// 1. Đọc latest.json
// -------------------------------------
$latestVersion = null;
$latestManifest = null;

$latestFile = $_SERVER["DOCUMENT_ROOT"] . "/ota/stm32/latest.json";

if (file_exists($latestFile)) {
    $latestJson = json_decode(file_get_contents($latestFile), true);
    if (is_array($latestJson)) {
        $latestVersion  = $latestJson["latest"]   ?? null;
        $latestManifest = $latestJson["manifest"] ?? null;
    }
}

// -------------------------------------
// 2. Query firmware versions từ DB
// -------------------------------------
$sql = "
    SELECT
        version,
        name,
        file_path,
        size,
        crc32,
        siglen,
        build_time,
        uploaded_at,
        description
    FROM firmware_versions
    ORDER BY uploaded_at DESC
";

$stmt = $conn->prepare($sql);
$stmt->execute();

$data = [];

while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {

    // Convert absolute path → public URL
    $publicPath = str_replace(
        $_SERVER["DOCUMENT_ROOT"],
        "",
        str_replace("\\", "/", $row["file_path"])
    );

    $data[] = [
        "version"     => $row["version"],
        "file_name"   => $row["name"],
        "file_url"    => $publicPath,
        "size_kb"     => round($row["size"] / 1024, 1),
        "uploaded_at" => $row["uploaded_at"],
        "description" => $row["description"],
        "is_latest"   => ($row["version"] === $latestVersion),
        "metadata" => [
            "CRC32"     => $row["crc32"],
            "SIGLEN"    => $row["siglen"],
            "BUILDTIME" => $row["build_time"]
        ]
    ];
}

// -------------------------------------
// 3. Trả JSON cho frontend
// -------------------------------------
echo json_encode([
    "status" => "ok",
    "latest" => [
        "version"  => $latestVersion,
        "manifest" => $latestManifest
    ],
    "data" => $data
], JSON_PRETTY_PRINT);

exit;
