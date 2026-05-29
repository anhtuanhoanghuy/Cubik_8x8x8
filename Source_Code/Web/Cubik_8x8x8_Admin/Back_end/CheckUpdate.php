<?php
header("Content-Type: application/json; charset=UTF-8");

// -------------------------------------
// 1. Đọc latest.json
// -------------------------------------
$latestFile = $_SERVER["DOCUMENT_ROOT"] . "/ota/stm32/latest.json";

if (!file_exists($latestFile)) {
    echo json_encode([
        "status"  => "error",
        "message" => "latest.json not found"
    ]);
    exit;
}

$latestJson = json_decode(file_get_contents($latestFile), true);

if (!is_array($latestJson) || !isset($latestJson["latest"])) {
    echo json_encode([
        "status"  => "error",
        "message" => "Invalid latest.json format"
    ]);
    exit;
}

// -------------------------------------
// 2. Trả JSON cho ESP / frontend
// -------------------------------------
echo json_encode([
    "status" => "ok",
    "latest" => [
        "version"  => $latestJson["latest"],
        "manifest" => $latestJson["manifest"] ?? null
    ]
], JSON_PRETTY_PRINT);

exit;
?>