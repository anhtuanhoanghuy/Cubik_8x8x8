<?php
session_start();

$base = "/Cubik_8x8x8/Source_Code/Web/Cubik_8x8x8_Admin";

// Nếu chưa login → về trang Login
if (!isset($_SESSION["account"])) {
    header("Location: $base/Front_end/HTML/Login.php");
    exit();
}

// Nếu login rồi → vào trang Index
header("Location: $base/Front_end/HTML/Index.html");
exit();
?>
