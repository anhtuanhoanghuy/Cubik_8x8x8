<?php
session_start();

// Xóa toàn bộ session
session_unset();
session_destroy();

// Chuyển hướng về trang login
header("Location: /Cubik_8x8x8/Source_Code/Web/Cubik_8x8x8_Admin/Front_end/HTML/Login.php");
exit();
?>
