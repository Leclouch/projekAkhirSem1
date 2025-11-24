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

// Struct untuk menyimpan item yang dibeli di keranjang
struct CartItem {
    string name;      // Nama menu yang dibeli
    int price;        // Harga per item
    int quantity;     // Jumlah yang dibeli
    int subtotal;     // Total harga untuk item ini (price * quantity)
};

// Class untuk mengelola seluruh sistem kantin
class Kantin {
    vector<MenuItem> menu;  // Menyimpan semua menu items
    vector<CartItem> cart;  // Keranjang belanja untuk menyimpan item yang dibeli
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

        tampilMenu();  // Tampilkan menu terlebih dahulu

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

    // Menampilkan isi keranjang belanja
    void tampilCart() {
        if (cart.empty()) {
            cout << "Keranjang belanja kosong.\n";
            return;
        }
        
        cout << "\n=== Keranjang Belanja ===\n";
        int totalHarga = 0;
        for (int i = 0; i < cart.size(); i++) {
            cout << (i+1) << ". " << cart[i].name << " x" << cart[i].quantity 
                 << " @ Rp" << cart[i].price << " = Rp" << cart[i].subtotal << "\n";
            totalHarga += cart[i].subtotal;
        }
        cout << "------------------------\n";
        cout << "Total: Rp" << totalHarga << "\n";
    }

    // Menambahkan item ke keranjang belanja
    void tambahKeCart() {
        int id, qty;
        cout << "ID menu yang dipilih: "; cin >> id;
        

        // Cari menu dengan ID yang sesuai
        for (auto &m : menu) {
            if (m.id == id) {

                cout << "Jumlah beli: "; cin >> qty;
                // Cek apakah stok cukup
                if (m.stock < qty) {
                    cout << "Stok tidak cukup. Stok tersedia: " << m.stock << "\n";
                    return;
                }
                
                // Kurangi stok
                m.stock -= qty;
                
                // Buat CartItem dan tambahkan ke keranjang
                CartItem item;
                item.name = m.name;
                item.price = m.price;
                item.quantity = qty;
                item.subtotal = qty * m.price;
                
                cart.push_back(item);
                cout << item.name << " ditambahkan ke keranjang.\n";
                return;
            }
        }
        cout << "Menu tidak ditemukan.\n";
    }

    // Menghapus item dari keranjang
    void hapusDariCart() {
        if (cart.empty()) {
            cout << "Keranjang belanja kosong.\n";
            return;
        }
        
        tampilCart();  // Tampilkan keranjang terlebih dahulu
        
        int idx;
        cout << "Nomor item yang ingin dihapus (0 untuk batal): ";
        cin >> idx;
        
        if (idx > 0 && idx <= cart.size()) {
            // Kembalikan stok ke menu
            for (auto &m : menu) {
                if (m.name == cart[idx-1].name) {
                    m.stock += cart[idx-1].quantity;
                    break;
                }
            }
            cart.erase(cart.begin() + idx - 1);
            cout << "Item dihapus dari keranjang.\n";
        } else if (idx != 0) {
            cout << "Nomor tidak valid.\n";
        }
    }

    // Membuat struk dan menyimpan ke file
    void buatStruk() {
        if (cart.empty()) {
            cout << "Keranjang belanja kosong. Tidak ada yang dibeli.\n";
            return;
        }
        
        tampilCart();  // Tampilkan keranjang sekali lagi sebelum checkout
        
        // Buat file struk pembelian
        ofstream out("receipt.txt");
        out << "=== STRUK PEMBELIAN ===\n";
        out << "ID Transaksi: " << nextTransId++ << "\n";
        out << "========================\n";
        
        int totalHarga = 0;
        // Tulis setiap item yang dibeli
        for (int i = 0; i < cart.size(); i++) {
            out << (i+1) << ". " << cart[i].name << " x" << cart[i].quantity 
                << " @ Rp" << cart[i].price << " = Rp" << cart[i].subtotal << "\n";
            totalHarga += cart[i].subtotal;
        }
        
        out << "========================\n";
        out << "Total: Rp" << totalHarga << "\n";
        out << "========================\n";
        out.close();

        cout << "\nTransaksi berhasil. Struk dicetak ke receipt.txt\n";
        cart.clear();  // Kosongkan keranjang setelah checkout
    }

    // Menu transaksi untuk membeli multiple items
    void transaksi() {
        int pilihan;
        
        do {
            cout << "\n=== Menu Transaksi ===\n";
            cout << "1. Lihat menu\n";
            cout << "2. Tambah item ke keranjang\n";
            cout << "3. Lihat keranjang\n";
            cout << "4. Hapus item dari keranjang\n";
            cout << "5. Checkout (buat struk)\n";
            cout << "0. Batal\n";
            cout << "Pilih: ";
            cin >> pilihan;
            
            switch (pilihan) {
                case 1:
                    tampilMenu();
                    break;
                case 2:
                    tambahKeCart();
                    break;
                case 3:
                    tampilCart();
                    break;
                case 4:
                    hapusDariCart();
                    break;
                case 5:
                    buatStruk();
                    return;  // Keluar dari transaksi setelah checkout
                case 0:
                    cout << "Transaksi dibatalkan. Stok dikembalikan.\n";
                    // Kembalikan semua stok
                    for (auto &item : cart) {
                        for (auto &m : menu) {
                            if (m.name == item.name) {
                                m.stock += item.quantity;
                                break;
                            }
                        }
                    }
                    cart.clear();
                    return;
                default:
                    cout << "Pilihan tidak valid.\n";
            }
        } while (pilihan != 0);
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