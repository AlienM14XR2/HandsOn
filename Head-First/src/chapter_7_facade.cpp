/**
 * Facade パターン 
 * 
 * インタフェースをより完結にする。
 * 
 * Decorator パターン ... インタフェースを変更せずに責務を追加する。
 * Adapter パターン ... あるインタフェースを別のインタフェースに変換（適合させる）する。
 * 
 * Apapter パターンと Facade パターンの違い、それはその目的にある。
 * Adapter パターンは、あるインタフェースをクライアントが求めるインタフェースに変換することであり、
 * Facade パターンは、サブシステムに対する単純化されたインタフェースをクライアントに提供することにある。
 * ファサードはインタフェースを単純化するだけでなく、クライアントを構成要素のサブシステムから分離する。
 * 
 * スイートホームシアター これがサンプルらしい：）
 * 
 * OO 原則
 * 最小知識の原則
 * 
 * 最小知識の原則：直接の友達とだけやり取りをする。
 * 
 * Facade パターンと最小知識の原則
 * Client は HomeTheaterFacade という友達を一人だけ持ちます。
 * OOP では、友達を一人しか持たないことは「よい」ことです！
 * 
 * e.g. compile
 * g++ -O3 -std=c++20 -DDEBUG -pedantic-errors -Wall -Werror chapter_7_facade.cpp -o ../bin/main 
 * 
*/
#include <iostream>
#include <cassert>

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M, D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * サブシステム群
*/

class StreamingPlayer {
public:
    StreamingPlayer() {}
    StreamingPlayer(const StreamingPlayer& own) {*this = own;}
    ~StreamingPlayer() {
        ptr_lambda_debug<const char*,const int&>("DONE ... StreamingPlayer Destructor.",0);
    }

    void on() {
        ptr_lambda_debug<const char*,const int&>("Player On.",0);
    }
    void off() {
        ptr_lambda_debug<const char*,const int&>("Player Off.",0);
    }
    void play(const string& title) {
        ptr_lambda_debug<const char*,const string&>("Play ... ",title);
    }
    void stop() {
        ptr_lambda_debug<const char*,const int&>("Player Stop.",0);
    }
};

class Amplifier {
private:
    StreamingPlayer* player;
    size_t volume = 0;
public:
    Amplifier() {}
    Amplifier(const Amplifier& own) {*this = own;}
    ~Amplifier() {
        ptr_lambda_debug<const char*,const int&>("DONE ... Amplifier Destructor.",0);
    }

    void on() {
        ptr_lambda_debug<const char*,const int&>("Amplifier On.",0);
    }
    void off() {
        ptr_lambda_debug<const char*,const int&>("Amplifier Off.",0);
    }
    void setStreamingPlayer(StreamingPlayer& p) {
        player = &p;
    }
    void setSurroundSound() {
        ptr_lambda_debug<const char*,const int&>("Surround Sound.",0);
    }
    void setVolumne(const size_t& v) {
        volume = v;
    }

};

class Tuner {
public:
    Tuner() {}
    Tuner(const Tuner& own) {*this = own;}
    ~Tuner() {
        ptr_lambda_debug<const char*,const int&>("DONE ... Tuner Destructor.",0);
    }
};

class Projector {
public:
    Projector() {}
    Projector(const Projector& own) {*this = own;}
    ~Projector() {
        ptr_lambda_debug<const char*,const int&>("DONE ... Projector Destructor.",0);
    }

    void on() {
        ptr_lambda_debug<const char*,const int&>("Projector On.",0);
    }
    void wideScreenMode() {        
        ptr_lambda_debug<const char*,const int&>("Wide Screen.",0);
    }
    void off() {
        ptr_lambda_debug<const char*,const int&>("Projector Off.",0);
    }
};

class TheaterLights {
private:
    size_t brightness = 0;
public:
    TheaterLights() {}
    TheaterLights(const TheaterLights& own) {*this = own;}
    ~TheaterLights() {
        ptr_lambda_debug<const char*,const int&>("DONE ... TheaterLights Destructor.",0);
    }

    void dim(const size_t d) {
        brightness = d;
    }
    void on() {
        ptr_lambda_debug<const char*,const int&>("Theater Lights On.",0);
    }
};

class Screen {
public:
    Screen() {}
    Screen(const Screen& own) {*this = own;}
    ~Screen() {
        ptr_lambda_debug<const char*,const int&>("DONE ... Screen Destructor.",0);
    }

    void down() {
        ptr_lambda_debug<const char*,const int&>("Screen Down.",0);
    }
    void up() {
        ptr_lambda_debug<const char*,const int&>("Screen Up.",0);
    }
};

class PopcornPopper {
public:
    PopcornPopper() {}
    PopcornPopper(const PopcornPopper& own) {*this = own;}
    ~PopcornPopper() {
        ptr_lambda_debug<const char*,const int&>("DONE ... PopcornPopper Destructor.",0);
    }

    void on() {
        ptr_lambda_debug<const char*,const int&>("PopcornPopper On.",0);
    }
    void pop() {
        ptr_lambda_debug<const char*,const int&>("pop.",0);
    }
    void off() {
        ptr_lambda_debug<const char*,const int&>("PopcornPopper Off.",0);
    }
};

/**
 * ホームシアターファサードを構築する
 * 
 * HomeTheaterFacade の構築を行う。
 * まず、コンポジションを使ってファサードがサブシステムのすべての構成要素にアクセス
 * できるようにする。
 * 
 * あぁ、構成要素が多いから、それらを最初に作る必要があるな、一旦すべてメモしてみる。
 * - Amplifier          増幅器、アンプ。
 * - Tuner
 * - StreamingPlayer
 * - Projector
 * - TheaterLights
 * - Screen
 * - PopcornPopper
*/
class HomeTheaterFacade {
private:
    Amplifier amp;
    Tuner tuner;
    StreamingPlayer player;
    Projector projector;
    TheaterLights lights;
    Screen screen;
    PopcornPopper popper;
    HomeTheaterFacade() {}
public:
    // TODO 次のようなコンストラクタこそFactory パターンを使うべきなんだろうか。
    HomeTheaterFacade(
      Amplifier& a,
      Tuner& t,
      StreamingPlayer& sp,
      Projector& prj,
      TheaterLights& tl,
      Screen& scr,
      PopcornPopper& pp
    ) {
        amp = a;
        tuner = t;
        player = sp;
        projector = prj;
        lights = tl;
        screen = scr;
        popper = pp;
    }
    HomeTheaterFacade(const HomeTheaterFacade& own) {*this = own;}
    ~HomeTheaterFacade() {}

    // TODO 実装 ... ここに必要なメソッドを追加していく。
    // 単純化されたインタフェースを実装する
    void watchMovie(string& movie) {
        puts("映画を観る準備をします。");
        // コンポジション された各サブシステム、メンバ変数の必要なメソッドを呼び出す。
        popper.on();
        popper.pop();
        lights.dim(10);
        screen.down();
        projector.on();
        projector.wideScreenMode();
        amp.on();
        amp.setStreamingPlayer(player);
        amp.setSurroundSound();
        amp.setVolumne(5);
        player.on();
        player.play(movie);
    }

    void endMovie() {
        puts("ムービーシアターを停止します。");
        popper.off();
        lights.on();
        screen.up();
        projector.off();
        amp.off();
        player.stop();
        player.off();
    }

};

// Amplifier amp;
Amplifier* amplifierFactory() {
    return new Amplifier();
}
// Tuner tuner;
Tuner* tunerFactory() {
    return new Tuner();
}
// StreamingPlayer player;
StreamingPlayer* streamingPlayerFactory() {
    return new StreamingPlayer();
}
// Projector projector;
Projector* projectorFactory() {
    return new Projector();
}
// TheaterLights lights;
TheaterLights* theaterLightsFactory() {
    return new TheaterLights();
}
// Screen screen;
Screen* screenFactory() {
    return new Screen();
}
// PopcornPopper popper;
PopcornPopper* popcornPopperFactory() {
    return new PopcornPopper();
}

HomeTheaterFacade* homeTheaterFacadeFactory() {
    Amplifier* amp = amplifierFactory();
    Tuner* tuner = tunerFactory();
    StreamingPlayer* sp = streamingPlayerFactory();
    Projector* projector = projectorFactory();
    TheaterLights* tl = theaterLightsFactory();
    Screen* screen = screenFactory();
    PopcornPopper* pp = popcornPopperFactory();

    HomeTheaterFacade* facade = new HomeTheaterFacade(
        *amp,
        *tuner,
        *sp,
        *projector,
        *tl,
        *screen,
        *pp
    );

    delete amp;
    delete tuner;
    delete sp;
    delete projector;
    delete tl;
    delete screen;
    delete pp;

    return facade;
}

int test_HomeTheaterFacade() {
    puts("--- test_HomeTheaterFacade");
    try {
        HomeTheaterFacade* facade = homeTheaterFacadeFactory();
        string movie = "Ghost in The Shell";
        facade->watchMovie(movie);
        facade->endMovie();
        delete facade;
        return 0;
    } catch (exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

int main(void) {
    puts("=== START Facade パターン");
    if(1.00) {
        int ret = -1;
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",ret = test_HomeTheaterFacade());
        assert(ret == 0);
    }
    puts("Facade パターン === END");
    return 0;
}