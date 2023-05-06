

#ifndef RENDERER_HPP
#define RENDERER_HPP

#pragma once


#ifndef SFML_STATIC
#define SFML_STATIC
#include <SFML/Graphics.hpp>
#else
#include <SFML/Graphics.hpp>
#endif//SFML_STATIC

#include <cmath>


namespace m {

    const std::string shader_source = 
    "#version 330\n"
    "precision highp float;uniform vec2 resolution;uniform vec2 mouse;uniform vec2 upperLeft;uniform vec2 lowerRight;uniform int  max_iter;float d(vec2 a, vec2 b) {return sqrt(pow(a.x - b.x, 2.0) + pow(a.y - b.y, 2.0));}float mandelbrot(vec2 c) {vec2 z = vec2(0.0, 0.0);int i = 0;for(; i<= max_iter; ++i) {z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;if(d(z, vec2(0.0,0.0)) > 2.0)return float(i)/float(max_iter);}return 0;}vec3 color(float k) {float r = 9.0 * (1.0 - k) * k * k * k;float g = 15.0 * (1.0 - k) * (1.0 - k) * k * k;float b = 8.5 * (1.0 - k) * (1.0 - k) * (1.0 - k) * k;return vec3(r, g, b);}void main() {vec2 c = vec2(upperLeft.x + (lowerRight.x - upperLeft.x) * gl_FragCoord.x / resolution.x,upperLeft.y + (lowerRight.y - upperLeft.y) * gl_FragCoord.y / resolution.y);c.x *= resolution.x / resolution.y;float m = mandelbrot(c);vec2 mp = mouse/resolution;gl_FragColor = vec4(color(m), 1.0);}";

 

    struct renderer : sf::RenderWindow {
        public:
            renderer( const sf::String& title, const sf::VideoMode& vm = {1024, 512}, const sf::Uint32& style = sf::Style::Default) : 
                sf::RenderWindow(vm, title, style)
            {
               
                sf::Texture tex;

                tex.create(vm.width, vm.height);
                sprite.setTexture(tex);

                shader.loadFromMemory(shader_source, sf::Shader::Fragment);

                shader.setUniform("resolution", sf::Glsl::Vec2(vm.width, vm.height));
            }
            ~renderer() {}

            virtual int exec() {
                sf::Time dt = sf::Time::Zero;
                clk.restart();
                while( isOpen() ) {
                    dt += clk.restart();
                    handle_event();
                    update_frame(clk.restart());
                    if(dt >= tpf) {
                        render_frame(dt);
                        dt = sf::Time::Zero;
                    }
                }
                return 0;
            }
            virtual void handle_event(){
                while(pollEvent(evt)) {
                    switch(evt.type) {
                        case sf::Event::Closed:
                            close();
                            break;
                        case sf::Event::Resized:
                            setSize(sf::Vector2u(evt.size.width, evt.size.height));
                            shader.setUniform("resolution", sf::Glsl::Vec2(evt.size.width, evt.size.height));
                            break;
                        case sf::Event::KeyPressed:
                            switch (evt.key.code){
                            case sf::Keyboard::W:
                                ul.y += 10.f * (lr.y - ul.y) / (getSize().y ); 
                                lr.y += 10.f * (lr.y - ul.y) / (getSize().y );
                                break;

                            case sf::Keyboard::S:
                                ul.y -= 10.f * (lr.y - ul.y) / (getSize().y );
                                lr.y -= 10.f * (lr.y - ul.y) / (getSize().y );
                                break;
                            case sf::Keyboard::A:
                                ul.x -= 10.f * (lr.x - ul.x) / (getSize().x );
                                lr.x -= 10.f * (lr.x - ul.x) / (getSize().x );
                                break;
                            case sf::Keyboard::D:
                                ul.x += 10.f * (lr.x - ul.x) / (getSize().x );
                                lr.x += 10.f * (lr.x - ul.x) / (getSize().x );
                                break;
                            case sf::Keyboard::E:
                                max_iter += 5;
                                break;
                            case sf::Keyboard::Q:
                                max_iter -= 5;
                                if (max_iter < 0)
                                    max_iter = 2;
                                break;
                            default:
                                break;
                            }
                            break;
                        case sf::Event::MouseWheelScrolled:
                            if(evt.mouseWheelScroll.delta > 0) {
                                zoom += 0.01f * zoom;
                                ul.x -= zoom * (lr.x - ul.x) / 10.f;
                                ul.y += zoom * (ul.y - lr.y) / 10.f;
                                lr.x += zoom * (lr.x - ul.x) / 10.f;
                                lr.y -= zoom * (ul.y - lr.y) / 10.f;
                            } else {
                                zoom -= 0.01f * zoom;
                                ul.x += zoom * (lr.x - ul.x) / 10.f;
                                ul.y -= zoom * (ul.y - lr.y) / 10.f;
                                lr.x -= zoom * (lr.x - ul.x) / 10.f;
                                lr.y += zoom * (ul.y - lr.y) / 10.f;
                            }                            
                            break;

                        default:
                            break;
                    }
                }
            }
            virtual void render_frame(const sf::Time& dt) {
                if(shader.isAvailable()){
                    draw(sprite, &shader);
                } else {
                    draw(sprite);
                }

                setTitle(" time per frame " + std::to_string( dt.asSeconds()) + " ITERATION " + std::to_string(max_iter));
                display();
            }
            virtual void update_frame(const sf::Time& dt) {
                sf::Vector2i mpos = sf::Mouse::getPosition(*this);

                shader.setUniform("mouse", sf::Glsl::Vec2(mpos.x, getSize().y - mpos.y));
                shader.setUniform("upperLeft", sf::Glsl::Vec2(ul.x, ul.y));
                shader.setUniform("lowerRight", sf::Glsl::Vec2(lr.x, lr.y));
                shader.setUniform("max_iter", max_iter);

            } 

        protected:
            sf::Vector2f    ul = {-2.0, 2.0};
            sf::Vector2f    lr = {2.0, -2.0};

            double          zoom = 1.0f;
            int max_iter    = 128;


            sf::Event   evt;
            sf::Clock   clk;
            sf::Time    tpf = sf::seconds(1.f/144.f);

            sf::Shader  shader;
            sf::Sprite  sprite;
    };

}

#endif//RENDERER_HPP