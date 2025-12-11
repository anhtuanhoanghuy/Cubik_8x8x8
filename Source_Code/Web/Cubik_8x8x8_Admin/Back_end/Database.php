<?php
// Thông tin cấu hình
$sName   = 'localhost';
$uName   = 'root';
$pass    = '';
$db_name = 'cubik_8x8x8';

// Biến kết nối
$conn = null;

// Hàm mở kết nối
function db_connect() {
    global $sName, $uName, $pass, $db_name, $conn;

    try {
        $conn = new PDO("mysql:host=$sName;dbname=$db_name", $uName, $pass);
        $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    } catch(PDOException $e) {
        echo "Connection failed: " . $e->getMessage();
    }

    return $conn;
}

// Hàm đóng kết nối
function db_disconnect() {
    global $conn;

    try {
        $conn = null;
    } catch (PDOException $e) {
        echo "Disconnect error: " . $e->getMessage();
    }
}
?>
