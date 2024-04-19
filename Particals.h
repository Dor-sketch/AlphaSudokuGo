#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <random>

class ParticleSystem : public sf::Drawable, public sf::Transformable {
public:
  ParticleSystem(unsigned int count, unsigned int windowWidth,
                 unsigned int windowHeight, sf::PrimitiveType type);
                 ParticleSystem(unsigned int count, unsigned int windowWidth,
                  unsigned int windowHeight, sf::PrimitiveType type, sf::Color color, float speed, float rand_speed);
  void setEmitter(sf::Vector2f position);
  void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
  void setMovingDirectionEffect(const sf::Vector2f &center);
  void update(sf::Time elapsed);
  void spawnParticles(unsigned int count);
  void resetParticle(std::size_t index);
  void changeColor(sf::Color color, sf::Color color2);
  void setParticlesType(sf::PrimitiveType type);
  void setSpeed(int base_speed, float rand_speed);
  struct Particle {
    sf::Vector2f velocity;
    sf::Time lifetime;
  };
  std::vector<Particle> m_particles;
  sf::VertexArray m_vertices;
  sf::Time m_lifetime;
  sf::Vector2f m_emitter;
  unsigned int count;
  unsigned int m_windowWidth;
  unsigned int m_windowHeight;
  sf::PrimitiveType m_type;
  float base_speed = 100;
  float rand_speed = 100;
};


class Effects {
public:
  Effects(sf::RenderTarget &target, sf::Clock &clock, unsigned int count,
          const std::vector<std::vector<char>> &numbers);
  void apply(const std::vector<std::vector<char>> &numbers);
  bool on = false;
  void apply(const int effect_type, const std::vector<std::vector<char>> &numbers);
  void next();
private:
  int current_effect = 0;
  sf::RenderTarget &target;
  sf::Clock &clock;
  unsigned int count;
  const std::vector<std::vector<char>> &numbers;
};