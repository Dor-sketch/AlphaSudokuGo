#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <random>

class ParticleSystem : public sf::Drawable, public sf::Transformable {
public:
  ParticleSystem(unsigned int count, unsigned int windowWidth,
                 unsigned int windowHeight);
  void setEmitter(sf::Vector2f position);
  void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
  void setMovingDirectionEffect(const sf::Vector2f &center);

  void update(sf::Time elapsed);

  void spawnParticles(unsigned int count);

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

  void resetParticle(std::size_t index);

  void changeColor(sf::Color color);
};

class DotsParticleSystem : public ParticleSystem {
public:
  DotsParticleSystem(unsigned int count, unsigned int windowWidth,
                     unsigned int windowHeight);

  void spawnParticles(unsigned int count);
  void setEmitter(sf::Vector2f position);

  void update(sf::Time elapsed);
  void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

  struct Particle {
    sf::Vector2f velocity;
    sf::Time lifetime;
  };
  void resetParticle(std::size_t index);
  void setMovingDirectionEffect();
  void setMovingDirectionEffect(const sf::Vector2f &center);

  void changeColor(sf::Color color);
  std::vector<Particle> m_particles;
  sf::VertexArray m_vertices;
  sf::Time m_lifetime;
  sf::Vector2f m_emitter;
  unsigned int m_windowWidth;
  unsigned int m_windowHeight;
  
};

class SmokeParticleSystem : public ParticleSystem {
public:
  SmokeParticleSystem(unsigned int count, unsigned int windowWidth,
                      unsigned int windowHeight);
  void spawnParticles(unsigned int count);

  // void resetParticle(std::size_t index) {
  //     // give a random velocity and lifetime to the particle
  //     float angle = (std::rand() % 360) * 3.14f / 180.f;
  //     float speed = (std::rand() % 50) + 50.f;
  //     m_particles[index].velocity =
  //         sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
  //     m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) +
  //     1000);

  //     // reset the position of the corresponding vertex
  //     m_vertices[index].position = m_emitter;
  // }

  void setEmitter(sf::Vector2f position);

  void update(sf::Time elapsed);
  void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
  void setMovingDirectionEffect(const sf::Vector2f &center);
  void setMovingDirectionEffect();

  void changeColor(sf::Color color);

  struct Particle {
    sf::Vector2f velocity;
    sf::Time lifetime;
  };

  void resetParticle(std::size_t index);
  std::vector<Particle> m_particles;
  sf::VertexArray m_vertices;
  sf::Time m_lifetime;
  sf::Vector2f m_emitter;
  unsigned int count;
};

class Effects {
public:
  Effects(sf::RenderTarget &target, sf::Clock &clock, unsigned int count,
          const std::vector<std::vector<char>> &numbers);
  void apply(const int effect_type,
             const std::vector<std::vector<char>> &numbers);

private:
  sf::RenderTarget &target;
  sf::Clock &clock;
  unsigned int count;
  const std::vector<std::vector<char>> &numbers;
};