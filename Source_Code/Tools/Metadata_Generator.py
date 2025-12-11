import tkinter as tk
from tkinter import filedialog, messagebox
import os
import zlib
from datetime import datetime

def embed_metadata(bin_path, output_path):
    with open(bin_path, 'rb') as f:
        data = f.read()
    
    size = len(data)
    crc32 = zlib.crc32(data) & 0xFFFFFFFF
    build_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    version = "1.0.0"

    meta_str = f"VERSION={version};SIZE={size};CRC32={crc32:08X};BUILDTIME={build_time};"
    meta_bytes = meta_str.encode('utf-8')
    meta_bytes += b'\0' * (64 - len(meta_bytes))

    with open(output_path, 'wb') as f:
        f.write(data)
        f.write(meta_bytes)

    # Trả về metadata để hiển thị
    return {
        "Version": version,
        "Size": size,
        "CRC32": f"{crc32:08X}",
        "Build Time": build_time
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
