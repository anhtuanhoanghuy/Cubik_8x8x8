import tkinter as tk
from tkinter import filedialog, messagebox
import os
import zlib
from datetime import datetime
import subprocess
import hashlib

#Lấy đường dẫn tuyệt đối của thư mục chứa tool
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

def embed_metadata(bin_path, output_path):
    # Đọc firmware gốc
    with open(bin_path, 'rb') as f:
        firmware = f.read()

    size = len(firmware)
    crc32 = zlib.crc32(firmware) & 0xFFFFFFFF
    build_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    # ✅ Version tạm để cứng, Anh có thể sửa sau
    version = "3.0.0"

    # Tạo đường dẫn tuyệt đối
    openssl_path = os.path.join(BASE_DIR, "OpenSSL-Win64", "bin", "openssl.exe")
    key_path = os.path.join(BASE_DIR, "Key", "private.pem")
    signature_file = os.path.join(BASE_DIR, "signature.bin")

    # Ký SHA-256
    subprocess.run([
        openssl_path,
        "dgst", "-sha256",
        "-sign", key_path,
        "-out", signature_file,
        bin_path
    ], check=True)

    with open(signature_file, "rb") as f:
        signature = f.read()

    # Tạo metadata
    meta_str = (
        f"VERSION={version};"
        f"SIZE={size};"
        f"CRC32={crc32:08X};"
        f"SIGLEN={len(signature)};"
        f"BUILDTIME={build_time};"
    )

    # Prefix length 2 bytes (Little Endian)
    meta_bytes = meta_str.encode('utf-8')
    meta_len = len(meta_bytes)
    prefix = meta_len.to_bytes(2, byteorder="little")

    # ✅ Tạo thư mục STM32/Revision
    revision_dir = os.path.join(BASE_DIR, "..", "STM32", "Revision")
    revision_dir = os.path.abspath(revision_dir)
    os.makedirs(revision_dir, exist_ok=True)

    # ✅ Tạo thư mục version bên trong Revision
    version_dir = os.path.join(revision_dir, f"version_{version}")
    os.makedirs(version_dir, exist_ok=True)

    # ✅ Lấy 4 ký tự cuối của CRC32
    crc_short = f"{crc32:08X}"[-4:]

    # ✅ Tạo tên file output theo format output_0x____.bin
    output_file = os.path.join(version_dir, f"output_0x{crc_short}.bin")

    # Ghi file output
    with open(output_file, 'wb') as f:
        f.write(prefix)
        f.write(meta_bytes)
        f.write(signature)
        f.write(firmware)

    # ✅ Copy signature.bin vào thư mục version
    signature_copy_path = os.path.join(version_dir, "signature.bin")
    with open(signature_copy_path, "wb") as f:
        f.write(signature)

    # ✅ Xóa signature.bin tạm trong BASE_DIR
    if os.path.exists(signature_file):
        os.remove(signature_file)

    return {
        "Version": version,
        "Size": size,
        "CRC32": f"{crc32:08X}",
        "Signature Length": len(signature),
        "Build Time": build_time,
        "Version Folder": version_dir
    }

def select_file():
    file_path = filedialog.askopenfilename(filetypes=[("Binary files", "*.bin")])
    if file_path:
        entry_file.delete(0, tk.END)
        entry_file.insert(0, file_path)

def generate_metadata():
    bin_path = entry_file.get()
    if not bin_path or not os.path.isfile(bin_path):
        messagebox.showerror("Error", "File .bin không tồn tại hoặc không hợp lệ!")
        return
    
    dir_name = os.path.dirname(bin_path)
    base_name = os.path.basename(bin_path)
    output_file = os.path.join(dir_name, base_name.replace(".bin", "_with_meta.bin"))

    lbl_status.config(text="Đang nhúng metadata...")
    root.update_idletasks()

    try:
        metadata = embed_metadata(bin_path, output_file)
        # Hiển thị thông tin metadata
        lbl_status.config(text=f"Success! Output: {output_file}")
        txt_info.config(state='normal')
        txt_info.delete(1.0, tk.END)
        for key, value in metadata.items():
            txt_info.insert(tk.END, f"{key}: {value}\n")
        txt_info.config(state='disabled')
    except Exception as e:
        lbl_status.config(text=f"Error: {e}")
        messagebox.showerror("Error", str(e))

# --- GUI ---
root = tk.Tk()
root.title("Firmware Metadata Embedder")
root.geometry("500x400")
root.resizable(False, False)  # fix cứng kích thước

# Center window on screen
screen_width = root.winfo_screenwidth()
screen_height = root.winfo_screenheight()
x = (screen_width - 500) // 2
y = (screen_height - 200) // 2
root.geometry(f"500x400+{x}+{y}")

# Main frame, căn giữa
frame_main = tk.Frame(root)
frame_main.pack(expand=True)

tk.Label(frame_main, text="Select firmware .bin file:").pack(pady=5)

frame_entry = tk.Frame(frame_main)
frame_entry.pack(pady=5)

entry_file = tk.Entry(frame_entry, width=50)
entry_file.pack(side=tk.LEFT, padx=5)

btn_browse = tk.Button(frame_entry, text="Browse", command=select_file)
btn_browse.pack(side=tk.LEFT, padx=5)

btn_generate = tk.Button(frame_main, text="Generate Metadata", width=20, command=generate_metadata)
btn_generate.pack(pady=15)

lbl_status = tk.Label(frame_main, text="")
lbl_status.pack(pady=5)

tk.Label(frame_main, text="Metadata:").pack()
txt_info = tk.Text(frame_main, width=60, height=6, state='disabled')
txt_info.pack(pady=5)

root.mainloop()