#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

// ===== BASE CLASS - Item =====
// Kelas dasar untuk MenuItem dan CartItem
class Item {
protected:
    string name;      // Nama item
    int price;        // Harga item
    
public:
    Item() : price(0) {}
    Item(const string& n, int p) : name(n), price(p) {}
    
    virtual ~Item() {}
    
    // Getter methods
    string getName() const { return name; }
    int getPrice() const { return price; }
    
    // Setter methods
    void setName(const string& n) { name = n; }
    void setPrice(int p) { price = p; }
};

// ===== MENU ITEM CLASS =====
class MenuItem : public Item {
private:
    int id;           // ID unik untuk setiap menu
    int stock;        // Jumlah stok tersedia

public:
    MenuItem() : Item(), id(0), stock(0) {}
    MenuItem(int i, const string& n, int p, int s) 
        : Item(n, p), id(i), stock(s) {}

    // Getter methods
    int getId() const { return id; }
    int getStock() const { return stock; }
    
    // Setter methods
    void setId(int i) { id = i; }
    void setStock(int s) { stock = s; }
    
    // Tambah/kurang stok
    void decreaseStock(int qty) { stock -= qty; }
    void increaseStock(int qty) { stock += qty; }
    
    // Konversi ke string untuk file
    string toString() const {
        ostringstream os;
        os << id << "|" << name << "|" << price << "|" << stock;
        return os.str();
    }

    // Parse dari string
    static MenuItem fromString(const string& line) {
        MenuItem m;
        size_t p1 = line.find('|'), p2 = line.find('|', p1+1), p3 = line.find('|', p2+1);
        m.id = stoi(line.substr(0, p1));
        m.name = line.substr(p1+1, p2-p1-1);
        m.price = stoi(line.substr(p2+1, p3-p2-1));
        m.stock = stoi(line.substr(p3+1));
        return m;
    }
};

// ===== CART ITEM CLASS =====
class CartItem : public Item {
private:
    int quantity;     // Jumlah yang dibeli
    int subtotal;     // Total harga untuk item ini

public:
    CartItem() : Item(), quantity(0), subtotal(0) {}
    CartItem(const string& n, int p, int q) 
        : Item(n, p), quantity(q), subtotal(p * q) {}

    // Getter methods
    int getQuantity() const { return quantity; }
    int getSubtotal() const { return subtotal; }
    
    // Update subtotal ketika quantity berubah
    void setQuantity(int q) { 
        quantity = q; 
        subtotal = price * quantity;
    }
};

// ===== MENU MANAGER CLASS =====
// Mengelola semua operasi menu
class MenuManager {
private:
    vector<MenuItem> menu;  // Menyimpan semua menu items
    int nextMenuId;         // ID berikutnya untuk menu baru

public:
    MenuManager() : nextMenuId(1) {}

    // Memuat data menu dari file
    void loadMenu(const string& filename) {
        ifstream in(filename);
        
        if (!in) {
            cout << "File " << filename << " tidak ditemukan, mulai dengan menu kosong.\n";
            return;
        }
        
        string line;
        while (getline(in, line)) {
            if (line.empty()) continue;
            MenuItem m = MenuItem::fromString(line);
            menu.push_back(m);
            if (m.getId() >= nextMenuId) nextMenuId = m.getId() + 1;
        }
        in.close();
        cout << "Menu berhasil dimuat dari " << filename << ".\n";
    }

    // Menambahkan menu item baru
    void tambahMenu() {
        MenuItem m;
        m.setId(nextMenuId++);
        
        cout << "Nama menu: "; 
        cin.ignore();
        string name;
        getline(cin, name);
        m.setName(name);
        
        cout << "Harga: "; 
        int price;
        cin >> price;
        m.setPrice(price);
        
        cout << "Stok: "; 
        int stock;
        cin >> stock;
        m.setStock(stock);
        
        menu.push_back(m);
        cout << "Menu ditambahkan.\n";
    }

    // Menampilkan semua menu
    void tampilMenu() const {
        cout << "\n=== Daftar Menu ===\n";
        for (const auto &m : menu) {
            cout << m.getId() << ". " << m.getName() << " - Rp" << m.getPrice()
                 << " (stok " << m.getStock() << ")\n";
        }
    }

    // Mengedit menu
    void editMenu() {
        tampilMenu();
        cout << "0. batalkan aksi\n";
        
        int id;
        cout << "\nID menu yang ingin diedit: ";
        cin >> id;
         if (id == 0)
        {
            cout << "Aksi dibatalkan.\n";
            return;
        }
        
        for (auto &m : menu) {
            if (m.getId() == id) {
                cout << "Nama baru (" << m.getName() << "): "; 
                cin.ignore();
                string name;
                getline(cin, name);
                m.setName(name);
                
                cout << "Harga baru (" << m.getPrice() << "): "; 
                int price;
                cin >> price;
                m.setPrice(price);
                
                cout << "Stok baru (" << m.getStock() << "): "; 
                int stock;
                cin >> stock;
                m.setStock(stock);
                
                cout << "Menu diperbarui.\n";
                return;
            }
        }
        cout << "Menu tidak ditemukan.\n";
    }

    // Menghapus menu
    void hapusMenu() {
        tampilMenu();
        cout << "0. batalkan aksi\n";
        
        int id;
        cout << "\nID menu yang ingin dihapus: ";
        cin >> id;
        if (id == 0)
        {
            cout << "Aksi dibatalkan.\n";
            return;
        }
        

        for (auto it = menu.begin(); it != menu.end(); ++it) {
            if (it->getId() == id) {
                menu.erase(it);
                cout << "Menu dihapus.\n";
                return;
            }
        }
        cout << "Menu tidak ditemukan.\n";
    }

    // Cari menu berdasarkan ID
    MenuItem* findMenuById(int id) {
        for (auto &m : menu) {
            if (m.getId() == id) {
                return &m;
            }
        }
        return nullptr;
    }

    // Getter untuk menu vector
    const vector<MenuItem>& getMenu() const {
        return menu;
    }
};

// ===== CART MANAGER CLASS =====
// Mengelola semua operasi keranjang belanja
class CartManager {
private:
    vector<CartItem> cart;  // Keranjang belanja

public:
    // Menampilkan keranjang
    void tampilCart() const {
        if (cart.empty()) {
            cout << "Keranjang belanja kosong.\n";
            return;
        }
        
        cout << "\n=== Keranjang Belanja ===\n";
        int totalHarga = hitungTotal();
        
        for (int i = 0; i < cart.size(); i++) {
            tampilItemCart(i);
        }
        
        cout << "------------------------\n";
        cout << "Total: Rp" << totalHarga << "\n";
    }

    // Helper method untuk menampilkan satu item cart
    void tampilItemCart(int index) const {
        cout << (index+1) << ". " << cart[index].getName() << " x" << cart[index].getQuantity() 
             << " @ Rp" << cart[index].getPrice() << " = Rp" << cart[index].getSubtotal() << "\n";
    }

    // Hitung total harga
    int hitungTotal() const {
        int total = 0;
        for (const auto &item : cart) {
            total += item.getSubtotal();
        }
        return total;
    }

    // Tambah item ke keranjang
    void tambahKeCart(MenuManager& menuManager) {
        menuManager.tampilMenu();
        
        int id;
        cout << "\nID menu yang dipilih: ";
        cin >> id;
        
        MenuItem* menuItem = menuManager.findMenuById(id);
        
        if (menuItem == nullptr) {
            cout << "Menu tidak ditemukan.\n";
            return;
        }
        
        int qty;
        cout << "Jumlah beli: ";
        cin >> qty;
        
        if (menuItem->getStock() < qty) {
            cout << "Stok tidak cukup. Stok tersedia: " << menuItem->getStock() << "\n";
            return;
        }
        
        // Kurangi stok
        menuItem->decreaseStock(qty);
        
        // Tambah ke keranjang
        CartItem item(menuItem->getName(), menuItem->getPrice(), qty);
        cart.push_back(item);
        cout << item.getName() << " ditambahkan ke keranjang.\n";
    }

    // Hapus item dari keranjang
    void hapusDariCart(MenuManager& menuManager) {
        if (cart.empty()) {
            cout << "Keranjang belanja kosong.\n";
            return;
        }
        
        tampilCart();
        
        int idx;
        cout << "Nomor item yang ingin dihapus (0 untuk batal): ";
        cin >> idx;
        
        if (idx > 0 && idx <= cart.size()) {
            // Kembalikan stok ke menu
            MenuItem* menuItem = menuManager.findMenuById(-1);  // Cari by name instead
            for (auto &m : menuManager.getMenu()) {
                if (m.getName() == cart[idx-1].getName()) {
                    const_cast<MenuItem&>(m).increaseStock(cart[idx-1].getQuantity());
                    break;
                }
            }
            
            cart.erase(cart.begin() + idx - 1);
            cout << "Item dihapus dari keranjang.\n";
        } else if (idx != 0) {
            cout << "Nomor tidak valid.\n";
        }
    }

    // Kembalikan semua stok ketika transaksi dibatalkan
    void returnAllStock(MenuManager& menuManager) {
        for (const auto &item : cart) {
            for (auto &m : menuManager.getMenu()) {
                if (m.getName() == item.getName()) {
                    const_cast<MenuItem&>(m).increaseStock(item.getQuantity());
                    break;
                }
            }
        }
        cart.clear();
    }

    // Kosongkan keranjang
    void clearCart() {
        cart.clear();
    }

    // Getter
    bool isEmpty() const {
        return cart.empty();
    }

    const vector<CartItem>& getCart() const {
        return cart;
    }
};

// ===== TRANSACTION MANAGER CLASS =====
// Mengelola semua operasi transaksi
class TransactionManager {
private:
    int nextTransId;  // ID transaksi berikutnya

public:
    TransactionManager() : nextTransId(1) {}

    // Membuat struk dan simpan ke file
    void buatStruk(CartManager& cartManager) {
        if (cartManager.getCart().empty()) {
            cout << "Keranjang belanja kosong. Tidak ada yang dibeli.\n";
            return;
        }
        
        cartManager.tampilCart();
        
        // Buat file struk pembelian
        ofstream out("receipt.txt");
        out << "=== STRUK PEMBELIAN ===\n";
        out << "ID Transaksi: " << nextTransId++ << "\n";
        out << "========================\n";
        
        int totalHarga = 0;
        const auto& cart = cartManager.getCart();
        
        for (int i = 0; i < cart.size(); i++) {
            out << (i+1) << ". " << cart[i].getName() << " x" << cart[i].getQuantity() 
                << " @ Rp" << cart[i].getPrice() << " = Rp" << cart[i].getSubtotal() << "\n";
            totalHarga += cart[i].getSubtotal();
        }
        
        out << "========================\n";
        out << "Total: Rp" << totalHarga << "\n";
        out << "========================\n";
        out.close();

        cout << "\nTransaksi berhasil. Struk dicetak ke receipt.txt\n";
        cartManager.clearCart();
    }
};

// ===== KANTIN CLASS (Main Orchestrator) =====
// Mengatur semua manager dan flow aplikasi
class Kantin {
private:
    MenuManager menuManager;
    CartManager cartManager;
    TransactionManager transactionManager;

public:
    Kantin() {}

    // Load menu saat startup
    void initialize() {
        menuManager.loadMenu("input.txt");
    }

    // Menu transaksi
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
                    menuManager.tampilMenu();
                    break;
                case 2:
                    cartManager.tambahKeCart(menuManager);
                    break;
                case 3:
                    cartManager.tampilCart();
                    break;
                case 4:
                    cartManager.hapusDariCart(menuManager);
                    break;
                case 5:
                    transactionManager.buatStruk(cartManager);
                    return;
                case 0:
                    cout << "Transaksi dibatalkan. Stok dikembalikan.\n";
                    cartManager.returnAllStock(menuManager);
                    return;
                default:
                    cout << "Pilihan tidak valid.\n";
            }
        } while (pilihan != 0);
    }

    // Menu utama
    void run() {
        int pilihan;
        
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

            switch (pilihan) {
                case 1: menuManager.tambahMenu(); break;
                case 2: menuManager.editMenu(); break;
                case 3: menuManager.hapusMenu(); break;
                case 4: menuManager.tampilMenu(); break;
                case 5: transaksi(); break;
            }
        } while (pilihan != 0);
    }
};

int main() {
    Kantin kantin;
    kantin.initialize();
    kantin.run();
    
    return 0;
}