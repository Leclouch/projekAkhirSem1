#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

// Struct untuk menyimpan data satu item menu
struct MenuItem {
    int id;           // ID unik untuk setiap menu
    string name;      // Nama menu
    int price;        // Harga menu
    int stock;        // Jumlah stok tersedia

    // Mengubah data MenuItem menjadi string dengan format "id|name|price|stock"
    // Digunakan untuk menyimpan ke file
    string toString() const {
        ostringstream os;
        os << id << "|" << name << "|" << price << "|" << stock;
        return os.str();
    }

    // Mengubah string format "id|name|price|stock" menjadi objek MenuItem
    // Contoh input: "1|Nasi Goreng|15000|10"
    static MenuItem fromString(const string& line) {
        MenuItem m{};  // Buat MenuItem kosong
        
        // Cari posisi semua karakter '|' (pipe) di dalam string
        size_t p1 = line.find('|'),        // Posisi pipe pertama
               p2 = line.find('|', p1+1),  // Posisi pipe kedua
               p3 = line.find('|', p2+1);  // Posisi pipe ketiga
        
        // Ambil substring antara setiap pipe dan konversi ke tipe data yang sesuai
        m.id = stoi(line.substr(0, p1));                    // Ambil ID (dari awal sampai pipe 1)
        m.name = line.substr(p1+1, p2-p1-1);               // Ambil nama (antara pipe 1 dan 2)
        m.price = stoi(line.substr(p2+1, p3-p2-1));        // Ambil harga (antara pipe 2 dan 3)
        m.stock = stoi(line.substr(p3+1));                 // Ambil stok (setelah pipe 3)
        return m;
    }
};

// Class untuk mengelola seluruh sistem kantin
class Kantin {
    vector<MenuItem> menu;  // Menyimpan semua menu items
    int nextMenuId = 1;     // ID berikutnya yang akan diberikan ke menu baru
    int nextTransId = 1;    // ID transaksi berikutnya
    
public:
    // Memuat data menu dari file input.txt
    void loadMenu(const string& filename) {
        ifstream in(filename);  // Buka file untuk dibaca
        
        // Jika file tidak ditemukan, tampilkan pesan dan mulai dengan menu kosong
        if (!in) {
            cout << "File " << filename << " tidak ditemukan, mulai dengan menu kosong.\n";
            return;
        }
        
        string line;
        // Baca file baris per baris
        while (getline(in, line)) {
            if (line.empty()) continue;  // Lewati baris kosong
            
            // Ubah string menjadi MenuItem menggunakan fungsi fromString
            MenuItem m = MenuItem::fromString(line);
            menu.push_back(m);  // Tambahkan ke vector menu
            
            // Update nextMenuId agar selalu lebih besar dari ID yang sudah ada
            if (m.id >= nextMenuId) nextMenuId = m.id + 1;
        }
        cout << "Menu berhasil dimuat dari " << filename << ".\n";
    }

    // Menambahkan menu item baru
    void tambahMenu() {
        MenuItem m;
        m.id = nextMenuId++;  // Berikan ID otomatis dan increment untuk menu berikutnya
        
        // Minta input dari user
        cout << "Nama menu: "; 
        cin.ignore();  // Bersihkan buffer input
        getline(cin, m.name);  // Gunakan getline untuk membaca nama dengan spasi
        cout << "Harga: "; cin >> m.price;
        cout << "Stok: "; cin >> m.stock;
        
        menu.push_back(m);  // Tambahkan menu baru ke vector
        cout << "Menu ditambahkan.\n";
    }

    // Mengedit menu item yang sudah ada
    void editMenu() {
        int id;
        cout << "ID menu yang ingin diedit: ";
        cin >> id;
        
        // Cari menu dengan ID yang sesuai
        for (auto &m : menu) {
            if (m.id == id) {
                // Tampilkan nilai lama dalam kurung dan minta input baru
                cout << "Nama baru (" << m.name << "): "; 
                cin.ignore();  // Bersihkan buffer input
                getline(cin, m.name);  // Gunakan getline untuk membaca nama dengan spasi
                cout << "Harga baru (" << m.price << "): "; cin >> m.price;
                cout << "Stok baru (" << m.stock << "): "; cin >> m.stock;
                cout << "Menu diperbarui.\n";
                return;  // Keluar dari fungsi setelah berhasil diupdate
            }
        }
        // Jika loop selesai tanpa menemukan ID yang cocok
        cout << "Menu tidak ditemukan.\n";
    }

    // Menghapus menu item
    void hapusMenu() {
        int id;
        cout << "ID menu yang ingin dihapus: ";
        cin >> id;
        
        // Gunakan iterator untuk loop karena kita perlu menghapus elemen
        for (auto it = menu.begin(); it != menu.end(); ++it) {
            if (it->id == id) {
                menu.erase(it);  // Hapus element dari vector
                cout << "Menu dihapus.\n";
                return;  // Keluar setelah berhasil dihapus
            }
        }
        // Jika loop selesai tanpa menemukan ID yang cocok
        cout << "Menu tidak ditemukan.\n";
    }

    // Menampilkan semua menu items dengan format yang rapi
    void tampilMenu() {
        cout << "\n=== Daftar Menu ===\n";
        
        // Loop melalui semua menu dan tampilkan dengan format tertentu
        for (auto &m : menu) {
            // Format: ID. Nama - RpHarga (stok Jumlah)
            cout << m.id << ". " << m.name << " - Rp" << m.price
                 << " (stok " << m.stock << ")\n";
        }
    }

    // Memproses transaksi pembelian
    void transaksi() {
        int id, qty;
        cout << "ID menu yang dipilih: "; cin >> id;
        cout << "Jumlah beli: "; cin >> qty;

        // Cari menu dengan ID yang sesuai
        for (auto &m : menu) {
            if (m.id == id) {
                // Cek apakah stok cukup
                if (m.stock < qty) {
                    cout << "Stok tidak cukup.\n";
                    return;
                }
                
                // Hitung total harga
                int total = qty * m.price;
                m.stock -= qty;  // Kurangi stok setelah pembelian

                // Buat file struk pembelian
                ofstream out("receipt.txt");
                out << "=== STRUK PEMBELIAN ===\n";
                out << "ID Transaksi: " << nextTransId++ << "\n";
                out << "Menu: " << m.name << "\n";
                out << "Harga: Rp" << m.price << "\n";
                out << "Jumlah: " << qty << "\n";
                out << "Total: Rp" << total << "\n";
                out << "=======================\n";
                out.close();

                cout << "Transaksi berhasil. Struk dicetak ke receipt.txt\n";
                return;
            }
        }
        // Jika menu tidak ditemukan
        cout << "Menu tidak ditemukan.\n";
    }
};

int main() {
    Kantin k;  // Buat object Kantin
    k.loadMenu("input.txt");  // Muat data menu dari file

    int pilihan;
    // Loop menu utama sampai user memilih keluar (0)
    do {
        cout << "\n=== Sistem Kantin ===\n";
        cout << "1. Tambah menu\n";
        cout << "2. Edit menu\n";
        cout << "3. Hapus menu\n";
        cout << "4. Tampilkan menu\n";
        cout << "5. Transaksi\n";
        cout << "0. Keluar\n";
        cout << "Pilih: ";
        cin >> pilihan;

        // Jalankan fungsi sesuai pilihan user
        switch (pilihan) {
            case 1: k.tambahMenu(); break;
            case 2: k.editMenu(); break;
            case 3: k.hapusMenu(); break;
            case 4: k.tampilMenu(); break;
            case 5: k.transaksi(); break;
        }
    } while (pilihan != 0);  // Ulangi sampai user pilih 0
    
    return 0;
}