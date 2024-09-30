#include <iostream>
#include <fstream>
#include <vector>
#include <limits>

using namespace std;

struct YanginBolgesi {
    string isim;
    string temsili;
    int siddet;
};

class Orman {
private:
    vector<vector<int>> ormanGraf;
    vector<string> bolgeIsimleri;
    vector<YanginBolgesi> yanginBolgeleri;
    int uygulananYangin;

public:
    Orman() : uygulananYangin(0) {}

    void ormanOku(const string& ormanDosyaAdi, const string& yanginBolgesiDosyaAdi);
    void ormanGrafiniYazdir();
    void yanginSondurmeSimulasyonu();
    void yakitHesapla(int& toplamYakit, int bolgeIndis);
    void suHesapla(int& toplamSu, int bolgeIndis, int& yanginSuresi);
    void mesafeHesapla(int& toplamMesafe, int bolge1Indis, int bolge2Indis);
    vector<int> dijkstra(int baslangic, int hedef);
};

void Orman::ormanOku(const string& ormanDosyaAdi, const string& yanginBolgesiDosyaAdi) {
    ifstream ormanDosya(ormanDosyaAdi);
    if (!ormanDosya.is_open()) {
        cerr << "Hata: Orman dosyası okunamıyor." << endl;
        exit(EXIT_FAILURE);
    }

    ifstream yanginBolgesiDosya(yanginBolgesiDosyaAdi);
    if (!yanginBolgesiDosya.is_open()) {
        cerr << "Hata: Yangın bölgeleri dosyası okunamıyor." << endl;
        exit(EXIT_FAILURE);
    }

    ormanGraf.clear();
    bolgeIsimleri.clear();
    yanginBolgeleri.clear();

    int boyut;
    ormanDosya >> boyut;
    ormanGraf.resize(boyut, vector<int>(boyut, 0));

    for (int i = 0; i < boyut; ++i) {
        for (int j = 0; j < boyut; ++j) {
            ormanDosya >> ormanGraf[i][j];
        }
    }

    string bolgeIsim;
    while (yanginBolgesiDosya >> bolgeIsim) {
        YanginBolgesi yanginBolgesi;
        yanginBolgesi.isim = bolgeIsim;
        yanginBolgesiDosya >> yanginBolgesi.temsili >> yanginBolgesi.siddet;

        bolgeIsimleri.push_back(bolgeIsim);
        yanginBolgeleri.push_back(yanginBolgesi);
    }

    ormanDosya.close();
    yanginBolgesiDosya.close();
}

void Orman::ormanGrafiniYazdir() {
    cout << "Orman Grafiği:" << endl;
    for (const auto& row : ormanGraf) {
        for (int weight : row) {
            cout << weight << "\t";
        }
        cout << endl;
    }

    cout << "Yangın Bölgeleri:" << endl;
    cout << "Yangin Bolgesi\tTemsili\tYangin Siddeti" << endl;
    for (const auto& yanginBolgesi : yanginBolgeleri) {
        cout << yanginBolgesi.isim << "\t" << yanginBolgesi.temsili << "\t" << yanginBolgesi.siddet << endl;
    }
}

void Orman::yakitHesapla(int& toplamYakit, int bolgeIndis) {
    toplamYakit += 10 * ormanGraf[0][bolgeIndis];
}

void Orman::suHesapla(int& toplamSu, int bolgeIndis, int& yanginSuresi) {
    YanginBolgesi yanginBolgesi = yanginBolgeleri[bolgeIndis];
    int suHarcama = yanginBolgesi.siddet * 1000;
    toplamSu += suHarcama;

    while (suHarcama >= 1000 && yanginBolgesi.siddet > 1) {
        suHarcama -= 1000;
        yanginBolgesi.siddet--;
        yanginSuresi -= 10;
    }
}

void Orman::mesafeHesapla(int& toplamMesafe, int bolge1Indis, int bolge2Indis) {
    toplamMesafe += ormanGraf[bolge1Indis][bolge2Indis];
}

vector<int> Orman::dijkstra(int baslangic, int hedef) {
    vector<int> mesafeler(ormanGraf.size(), numeric_limits<int>::max());
    vector<bool> ziyaretEdildi(ormanGraf.size(), false);
    vector<int> onceki(ormanGraf.size(), -1);

    mesafeler[baslangic] = 0;

    for (int i = 0; i < ormanGraf.size(); ++i) {
        int u = -1;
        for (int j = 0; j < ormanGraf.size(); ++j) {
            if (!ziyaretEdildi[j] && (u == -1 || mesafeler[j] < mesafeler[u]))
                u = j;
        }

        if (mesafeler[u] == numeric_limits<int>::max())
            break;

        ziyaretEdildi[u] = true;

        for (int v = 0; v < ormanGraf.size(); ++v) {
            if (!ziyaretEdildi[v] && ormanGraf[u][v] > 0 && mesafeler[u] + ormanGraf[u][v] < mesafeler[v]) {
                mesafeler[v] = mesafeler[u] + ormanGraf[u][v];
                onceki[v] = u;
            }
        }
    }

    vector<int> yol;
    for (int v = hedef; v != -1; v = onceki[v]) {
        yol.push_back(v);
    }
    reverse(yol.begin(), yol.end());

    return yol;
}

void Orman::yanginSondurmeSimulasyonu() {
    int baslangic = 0;
    int hedef = 1;

    vector<int> enKisaYol = dijkstra(baslangic, hedef);

    cout << "Uçağın en kısa rota üzerindeki yangınları söndürme simülasyonu:" << endl;

    int toplamYakit = 0;
    int toplamSu = 0;
    int toplamMesafe = 0;

    for (size_t i = 0; i < enKisaYol.size(); ++i) {
        int bolge1Indis = enKisaYol[i];
        int bolge2Indis = (i + 1) % enKisaYol.size();

        mesafeHesapla(toplamMesafe, bolge1Indis, bolge2Indis);
        yakitHesapla(toplamYakit, bolge2Indis);

        int yanginSuresi = yanginBolgeleri[bolge2Indis].siddet * 10;
        suHesapla(toplamSu, bolge2Indis, yanginSuresi);

        uygulananYangin++;
    }

    cout << "Uçak, toplam " << uygulananYangin << " adet yangını söndürdü." << endl;
    cout << "Toplam Yakıt Kullanımı: " << toplamYakit << " lt" << endl;
    cout << "Toplam Su Kullanımı: " << toplamSu << " lt" << endl;
    cout << "Toplam Mesafe Kat Edilen: " << toplamMesafe << " km" << endl;
}

int main() {
    Orman orman;
    orman.ormanOku("orman.txt", "yangin_bolgeleri.txt");
    orman.ormanGrafiniYazdir();
    orman.yanginSondurmeSimulasyonu();

    return 0;
}
