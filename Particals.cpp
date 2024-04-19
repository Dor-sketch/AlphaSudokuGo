#include "Particals.h"
#include "GlowingColor.h"

unsigned int howManyCellsDrawn(const std::vector<std::vector<char>> &numbers) {
  unsigned int count = 0;
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (numbers[i][j] != '.') {
        count++;
      }
    }
  }
  return count;
}

ParticleSystem::ParticleSystem(unsigned int count, unsigned int windowWidth,
                               unsigned int windowHeight,
                               sf::PrimitiveType type)
    : count(count), m_particles(count), m_vertices(type, count * 2),
      m_lifetime(sf::seconds(3)), m_emitter(0, 0), m_windowWidth(windowWidth),
      m_windowHeight(windowHeight), m_type(type) {}

ParticleSystem::ParticleSystem(unsigned int count, unsigned int windowWidth,
                               unsigned int windowHeight, sf::PrimitiveType type,
                               sf::Color color1, float base_speed,
                               float rand_speed)
    : count(count), m_particles(count), m_vertices(type, count * 2),
      m_lifetime(sf::seconds(1)), m_emitter(0, 0), m_windowWidth(windowWidth),
      m_windowHeight(windowHeight), m_type(type), base_speed(base_speed),
      rand_speed(rand_speed) {
  for (std::size_t i = 0; i < m_particles.size(); ++i) {
    m_vertices[i * 2].color = color1;
    m_vertices[i * 2 + 1].color = color1;
  }
}

void ParticleSystem::setEmitter(sf::Vector2f position) { m_emitter = position; }
void ParticleSystem::draw(sf::RenderTarget &target,
                          sf::RenderStates states) const {
  states.transform *= getTransform();
  states.texture = NULL;
  target.draw(m_vertices, states);
}
void ParticleSystem::setMovingDirectionEffect(const sf::Vector2f &center) {
  for (std::size_t i = 0; i < m_particles.size(); ++i) {
    sf::Vector2f direction = m_vertices[i].position - center;
    float length =
        std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length != 0) {     // avoid division by zero
      direction /= length; // normalize the direction vector
    }
    // Make the speed proportional to the distance from the center
    float speed = length / 100.0f; // Adjust the divisor to control the speed
    m_particles[i].velocity = direction * speed;
  }
}

void ParticleSystem::update(sf::Time elapsed) {
  for (std::size_t i = 0; i < m_particles.size(); ++i) {
    // update the particle lifetime
    Particle &p = m_particles[i];
    p.lifetime -= elapsed;

    // if the particle is dead, respawn it
    if (p.lifetime <= sf::Time::Zero)
      resetParticle(i);

    // update the position of the corresponding vertex
    m_vertices[i * 2 + 1].position += p.velocity * elapsed.asSeconds();

    // update the alpha (transparency) of the particle according to its lifetime
    float ratio =
        std::max(p.lifetime.asSeconds() / m_lifetime.asSeconds(), 0.f);
    m_vertices[i * 2].color.a = static_cast<sf::Uint8>(ratio * 255);
    m_vertices[i * 2 + 1].color.a = static_cast<sf::Uint8>(ratio * 255);
  }
}

void ParticleSystem::spawnParticles(unsigned int count) {
  for (unsigned int i = 0; i < count; ++i) {
    resetParticle(i);
  }
}

void ParticleSystem::resetParticle(std::size_t index) {
  // give a random velocity and lifetime to the particle
  float angle = (std::rand() % 360) * 3.14159f / 180;
  float speed = (std::rand() % static_cast<int>(rand_speed)) + base_speed;
  m_particles[index].velocity =
      sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);

  // calculate the maximum distance a particle could need to travel (diagonal
  // across the screen)
  float maxDistance = std::sqrt(m_windowWidth * m_windowWidth +
                                m_windowHeight * m_windowHeight);

  // calculate the time it would take for the particle to travel that distance
  // at its current speed
  sf::Time maxLifetime = sf::seconds(maxDistance / speed);

  // set the particle's lifetime to a random value between std::rand() % 100 + 1
  // milliseconds and the calculated maximum
  m_particles[index].lifetime = sf::milliseconds(
      (std::rand() % static_cast<int>(maxLifetime.asMilliseconds())) +
      std::rand() % 100 + 1);

  // reset the position of the corresponding vertices
  m_vertices[index * 2].position =
      sf::Vector2f(std::rand() % m_windowWidth, std::rand() % m_windowHeight);
  m_vertices[index * 2 + 1].position = m_emitter;

  // set the particle's velocity to a vector pointing from the center to the
  // particle's position
  sf::Vector2f direction = m_vertices[index * 2].position - m_emitter;
  float length =
      std::sqrt(direction.x * direction.x + direction.y * direction.y);
  if (length != 0) {     // avoid division by zero
    direction /= length; // normalize the direction vector
  }
  m_particles[index].velocity = direction * speed;
}

void ParticleSystem::changeColor(sf::Color color1, sf::Color color2) {
  for (std::size_t i = 0; i < m_particles.size(); ++i) {
    m_vertices[i * 2].color = color1;
    m_vertices[i * 2 + 1].color = color2;
  }
}
void drawStars(sf::RenderTarget &target, sf::Clock &clock) {
  static ParticleSystem particles(1000, target.getSize().x, target.getSize().y,
                                  sf::Points);
  static sf::Clock spawnClock;

  // Spawn new particles every 100 milliseconds
  if (spawnClock.getElapsedTime() > sf::milliseconds(100)) {
    particles.spawnParticles(10); // Spawn 10 new particles
    spawnClock.restart();
  }

  // Update particles
  sf::Time elapsed = clock.restart();
  particles.update(elapsed);

  // Modify particle velocities using sine and cosine functions
  for (size_t i = 0; i < particles.m_particles.size(); ++i) {
    ParticleSystem::Particle &p = particles.m_particles[i];
    p.velocity.x = std::cos(elapsed.asSeconds() + i) * 100;
    p.velocity.y = std::sin(elapsed.asSeconds() + i) * 100;
  }

  target.draw(particles);
}
void MetShower(sf::RenderTarget &target, sf::Clock &clock) {
  static ParticleSystem particles(std::rand() % 100 + 1, target.getSize().x,
                                  target.getSize().y, sf::Lines);
  static sf::Clock spawnClock;

  // Spawn new particles at random intervals
  if (spawnClock.getElapsedTime() > sf::milliseconds(rand() % 200 + 50)) {
    particles.spawnParticles(rand() % 5 + 1); // Spawn a random number of particles
    spawnClock.restart();
  }

  // Update particles
  sf::Time elapsed = clock.restart();
  particles.update(elapsed);

  // Modify particle velocities and colors
  for (std::size_t i = 0; i < particles.m_vertices.getVertexCount(); ++i) {
    sf::Vertex &particle = particles.m_vertices[i];
    particle.position += particles.m_particles[i / 2].velocity * elapsed.asSeconds();
    particle.color.a = static_cast<sf::Uint8>(particle.color.a * 0.99f);
    particle.color.r = static_cast<sf::Uint8>(particle.color.r * 0.99f);
    particle.color.g = static_cast<sf::Uint8>(particle.color.g * 0.99f);
    particle.color.b = static_cast<sf::Uint8>(particle.color.b * 0.99f);

    // If the particle is out of the screen, reset its position to the center
    if (particle.position.x < 0 || particle.position.x > target.getSize().x ||
        particle.position.y < 0 || particle.position.y > target.getSize().y) {
      particle.position = particles.m_emitter;
    }

    // If the particle is too dim, reset its color to white
    if (particle.color.a < 10) {
      particle.color = sf::Color::White;
    }

    // If the particle is too dim, reset its color to white
  }

  target.draw(particles);
}
void drawGalaxy(sf::RenderTarget &target, sf::Clock &clock) {
  static ParticleSystem particles(1000, target.getSize().x, target.getSize().y,
                                  sf::Lines);
  static ParticleSystem particles2(1000, target.getSize().x,
                                   target.getSize().y, sf::Points);
  static sf::Clock spawnClock;
  static sf::Clock spawnClock2;

  // Spawn new particles at random intervals
  if (spawnClock.getElapsedTime() > sf::milliseconds(rand() % 1000 + 50)) {
    particles.spawnParticles(rand() % 1000 + 5); // Spawn a random number of particles
    spawnClock.restart();
  }
  if (spawnClock2.getElapsedTime() > sf::milliseconds(rand() % 200 + 50)) {
    particles2.spawnParticles(rand() % 20 + 5); // Spawn a random number of particles
    spawnClock2.restart();
  }

  // Update particles
  sf::Time elapsed = clock.restart();
  particles.update(elapsed);
  particles2.update(elapsed);

  // Modify particle velocities to create a circular motion
  for (std::size_t i = 0; i < particles.m_particles.size(); ++i) {
    ParticleSystem::Particle &p = particles.m_particles[i];
    float time = elapsed.asSeconds();
    p.velocity.x = std::cos(time) * 100.0f;
    p.velocity.y = std::sin(time) * 100.0f;
  }
  for (std::size_t i = 0; i < particles2.m_particles.size(); ++i) {
    ParticleSystem::Particle &p = particles2.m_particles[i];
    float time = elapsed.asSeconds();
    p.velocity.x = std::cos(time) * 100.0f;
    p.velocity.y = std::sin(time) * 100.0f;
  }
  particles.changeColor(GlowingColor(triadicYellow).getShade(),
                        GlowingColor(triadicYellow).getBrighterShade());

                        particles2.changeColor(GlowingColor(triadicYellow).getShade(),
                        GlowingColor(triadicYellow).getBrighterShade());

  particles.setEmitter(sf::Vector2f(target.getSize().x / 2, target.getSize().y / 2));
  particles2.setEmitter(sf::Vector2f(target.getSize().x / 2, target.getSize().y / 2));

// adjust alpha value of the particles so that the lines will be transparent when they are far from the center
for (std::size_t i = 0; i < particles.m_vertices.getVertexCount(); ++i) {
    sf::Vertex &particle = particles.m_vertices[i];
    auto direction = particle.position - particles.m_emitter;
    float length =
        std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction /= length;
    particle.position += direction * 2.f; // Slow down the speed by reducing the factor

    // Update the alpha (transparency) of the particle according to its distance
    // from the center
    float ratio = length / std::max(target.getSize().x, target.getSize().y);
    particle.color.a = static_cast<sf::Uint8>(
        (std::sin(ratio * 3.14159f * 2) + 1) / 2 *
        255); // Use a sine function to create a pulsating effect

    // If the particle is out of the screen, reset its position to the center
    if (particle.position.x < 0 || particle.position.x > target.getSize().x ||
        particle.position.y < 0 || particle.position.y > target.getSize().y) {
      particle.position = particles.m_emitter;
    }
  }



  target.draw(particles);
  target.draw(particles2);
}

void drawShootingStar(sf::RenderTarget &target, sf::Clock &clock) {
  static ParticleSystem particles(10000, target.getSize().x, target.getSize().y,
                                  sf::Lines);
  static sf::Clock spawnClock;

  // Spawn new particles every 100 milliseconds
  if (spawnClock.getElapsedTime() > sf::milliseconds(1000)) {
    particles.spawnParticles(1); // Spawn 1 new particle
    spawnClock.restart();
  }
  auto emit = sf::Vector2f(target.getSize().x / 2, target.getSize().y / 2);
  auto direction = sf::Vector2f(target.getSize().x / 2, target.getSize().y / 2);
  particles.update(clock.restart());
  particles.setEmitter(emit);
  particles.setMovingDirectionEffect(direction);

  for (std::size_t i = 0; i < particles.m_particles.size(); ++i) {
    ParticleSystem::Particle &p = particles.m_particles[i];
    p.velocity.x = std::rand() % 100 - 50;
    p.velocity.y = std::rand() % 100 - 50;
  }

  for (std::size_t i = 0; i < particles.m_vertices.getVertexCount(); ++i) {
    sf::Vertex &particle = particles.m_vertices[i];
    auto direction = particle.position - particles.m_emitter;
    float length =
        std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction /= length;
    particle.position += direction * 2.f; // Slow down the speed by reducing the factor

    // Update the alpha (transparency) of the particle according to its distance
    // from the center
    float ratio = length / std::max(target.getSize().x, target.getSize().y);
    particle.color.a = static_cast<sf::Uint8>(
        (std::sin(ratio * 3.14159f * 2) + 1) / 2 *
        255); // Use a sine function to create a pulsating effect

    // If the particle is out of the screen, reset its position to the center
    if (particle.position.x < 0 || particle.position.x > target.getSize().x ||
        particle.position.y < 0 || particle.position.y > target.getSize().y) {
      particle.position = particles.m_emitter;
    }
  }

  particles.changeColor(GlowingColor(triadicYellow).getShade(),
                        GlowingColor(triadicYellow).getBrighterShade());

  target.draw(particles);
}
void drawSmoke(sf::RenderTarget &target, sf::Clock &clock) {
  static ParticleSystem particles(1000, target.getSize().x, target.getSize().y,
                                  sf::Points, sf::Color::White, 100, 10);
  static sf::Clock spawnClock;

  // Spawn new particles at random intervals
  if (spawnClock.getElapsedTime() > sf::milliseconds(rand() % 200 + 50)) {
    particles.spawnParticles(rand() % 20 + 5); // Spawn a random number of particles
    spawnClock.restart();
  }

  // Update particles
  sf::Time elapsed = clock.restart();
  particles.update(elapsed);

  // Modify particle velocities and colors
  for (size_t i = 0; i < particles.m_vertices.getVertexCount(); ++i) {
    sf::Vertex &particle = particles.m_vertices[i];
    particle.position += particles.m_particles[i / 2].velocity * elapsed.asSeconds();
    particle.color.a = static_cast<sf::Uint8>(particle.color.a * 0.99f);
  }

  for (std::size_t i = 0; i < particles.m_particles.size(); ++i) {
    ParticleSystem::Particle &p = particles.m_particles[i];
    p.velocity.x = std::rand() % 100 - 50;
    p.velocity.y = std::rand() % 100 - 50;
  }
  target.draw(particles);
}


void drawFire(sf::RenderTarget &target, sf::Clock &clock) {
  static ParticleSystem particles1(10000, target.getSize().x, target.getSize().y,
                                  sf::Points, sf::Color::Red, 200, 20);
  static ParticleSystem particles2(10000, target.getSize().x, target.getSize().y,
                                  sf::Points, sf::Color::Yellow, 200, 20);
  static sf::Clock spawnClock;

  if (spawnClock.getElapsedTime() > sf::milliseconds(100000)) {
    particles1.spawnParticles(10);
    particles2.spawnParticles(10);
    spawnClock.restart();
  }
  particles1.setSpeed(100, 300);
  particles2.setSpeed(100, 300);
  particles1.changeColor(sf::Color::Red, sf::Color::Yellow);
  particles2.changeColor(sf::Color::Yellow, sf::Color::Red);
  particles1.setEmitter(sf::Vector2f(target.getSize().x / 2, target.getSize().y));
  particles2.setEmitter(sf::Vector2f(target.getSize().x / 2, target.getSize().y));
  particles1.setMovingDirectionEffect(sf::Vector2f(0, -1));
  particles2.setMovingDirectionEffect(sf::Vector2f(0, -1));

  particles1.update(clock.restart());
  particles2.update(clock.restart());
  // add dynamic effect to the particles add organic movement
  for (std::size_t i = 0; i < particles1.m_vertices.getVertexCount(); ++i) {
    sf::Vertex &particle = particles1.m_vertices[i];
    auto direction = particle.position - particles1.m_emitter;
    float length =
        std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction /= length;
    particle.position += direction * 2.f; // Slow down the speed by reducing the factor

    // Update the alpha (transparency) of the particle according to its distance
    // from the center
    float ratio = length / std::max(target.getSize().x, target.getSize().y);
    particle.color.a = static_cast<sf::Uint8>(
        (std::sin(ratio * 3.14159f * 2) + 1) / 2 *
        255); // Use a sine function to create a pulsating effect

    // If the particle is out of the screen, reset its position to the center
    if (particle.position.x < 0 || particle.position.x > target.getSize().x ||
        particle.position.y < 0 || particle.position.y > target.getSize().y) {
      particle.position = particles1.m_emitter;
    }
  }

  target.draw(particles1);
  target.draw(particles2);
}
void drawRain(sf::RenderTarget &target, sf::Clock &clock) {
  static ParticleSystem particles(1000, target.getSize().x, target.getSize().y,
                                  sf::Lines, sf::Color::Blue, 100, 50);
  static sf::Clock spawnClock;

  if (spawnClock.getElapsedTime() > sf::milliseconds(100)) {
    particles.spawnParticles(10);
    spawnClock.restart();
  }
  particles.setMovingDirectionEffect(sf::Vector2f(0, target.getSize().y));
  particles.setEmitter(sf::Vector2f(target.getSize().x / 2, 0));
  particles.update(clock.restart());
  target.draw(particles);
}
void drawAllAround(sf::RenderTarget &target, sf::Clock &clock, int cellSize,
                   const std::vector<std::vector<char>> &numbers) {
  static ParticleSystem particles(1000, target.getSize().x, target.getSize().y,
                                  sf::Lines, sf::Color::Green, 150, 30);
  static ParticleSystem particles2(1000, target.getSize().x, target.getSize().y,
                                   sf::Lines, sf::Color::Blue, 150, 30);
  static ParticleSystem particles3(1000, target.getSize().x, target.getSize().y,
                                    sf::Lines, sf::Color::Red, 150, 30);
  static sf::Clock spawnClock;

  if (spawnClock.getElapsedTime() > sf::milliseconds(100)) {
    particles.spawnParticles(rand() % 10 + 1);
    particles2.spawnParticles(rand() % 10 + 1);
    particles3.spawnParticles(rand() % 10 + 1);
    spawnClock.restart();
  }

  particles.setEmitter(sf::Vector2f(rand() % target.getSize().x, rand() % target.getSize().y));
  particles2.setEmitter(sf::Vector2f(rand() % target.getSize().x, rand() % target.getSize().y));
  particles3.setEmitter(sf::Vector2f(rand() % target.getSize().x, rand() % target.getSize().y));

  particles.update(clock.restart());
  particles2.update(clock.restart());
  particles3.update(clock.restart());
  target.draw(particles);
  target.draw(particles2);
  target.draw(particles3);
}

void colorsMode(sf::RenderTarget &target, sf::Clock &clock) {
  static ParticleSystem particles(1000, target.getSize().x, target.getSize().y,
                                  sf::Lines, sf::Color::Magenta, 2000, 400);
  static sf::Clock spawnClock;

  if (spawnClock.getElapsedTime() > sf::milliseconds(100)) {
    particles.spawnParticles(10);
    spawnClock.restart();
  }

  particles.update(clock.restart());
  target.draw(particles);
}

void drawColoredSkies(sf::RenderTarget &target, sf::Clock &clock) {
  auto num_of_particles = []() -> int { return std::rand() % 100 + 1; };
  static ParticleSystem particles(num_of_particles(), target.getSize().x,
                                  target.getSize().y, sf::Lines);
  static ParticleSystem particles2(10000, target.getSize().x,
                                   target.getSize().y, sf::Points);
  static ParticleSystem particles3(num_of_particles(), target.getSize().x,
                                   target.getSize().y, sf::Lines);
  auto glowingColor = GlowingColor(analogousCyan).getShade();
  auto glowingBrighterColor = GlowingColor(analogousCyan).getBrighterShade();
  static sf::Clock spawnClock;

  // Spawn new particles every 100 milliseconds
  if (spawnClock.getElapsedTime() > sf::milliseconds(100)) {
    particles.spawnParticles(1);  // Spawn 1 new particle
    particles2.spawnParticles(1); // Spawn 1 new particle
    particles3.spawnParticles(1); // Spawn 1 new particle
    spawnClock.restart();
  }
  particles.update(clock.restart());
  particles2.update(clock.restart());
  particles3.update(clock.restart());
  particles.changeColor(glowingColor, glowingBrighterColor);
  particles2.changeColor(glowingColor, glowingBrighterColor);
  particles2.setEmitter(
      sf::Vector2f(target.getSize().x / 2, target.getSize().y / 2));
  // particles3.changeColor(glowingColor, glowingBrighterColor);
  target.draw(particles);
}

void ParticleSystem::setSpeed(int base_speed, float rand_speed) {
  this->base_speed = base_speed;
  this->rand_speed = rand_speed;
}

void drawTunnel(sf::RenderTarget &target, sf::Clock &clock) {
  static ParticleSystem particles(1000, target.getSize().x, target.getSize().y,
                                  sf::Lines);
  particles.setSpeed(100, 300);
  static auto glowingColor = GlowingColor(complementaryPurple).getShade();
  static auto glowingBrighterColor =
      GlowingColor(complementaryPurple).getBrighterShade();
  static sf::Clock spawnClock;
  static sf::CircleShape circle(50);
  circle.setFillColor(sf::Color::Black);
  static sf::Vector2f center =
      circle.getPosition() +
      sf::Vector2f(50, 50); // Make center a static variable

  if (spawnClock.getElapsedTime() > sf::milliseconds(10000)) {
    particles.spawnParticles(1);
    spawnClock.restart();
  }

  // add moving effect to the center
  auto c = clock.restart();
  float elapsed = c.asSeconds();
  static auto movings = sf::Vector2f(std::sin(elapsed) * 0.8, std::cos(elapsed) * 0.5);
  center += movings;

  // Add some randomness to the movement direction and speed
  movings.x += ((std::rand() % 200 - 100) / 100.0) * 0.2;
  movings.y += ((std::rand() % 200 - 100) / 100.0) * 0.2;

  // Keep the movement within the screen bounds
  if (center.x < 0) {
    center.x = 0;
    movings.x = std::abs(movings.x);
  } else if (center.x > target.getSize().x) {
    center.x = target.getSize().x;
    movings.x = -std::abs(movings.x);
  }
  if (center.y < 0) {
    center.y = 0;
    movings.y = std::abs(movings.y);
  } else if (center.y > target.getSize().y) {
    center.y = target.getSize().y;
    movings.y = -std::abs(movings.y);
  }
  
  center += movings; // Apply the possibly updated movings vector
  particles.setEmitter(center);
  particles.update(c);
  particles.changeColor(glowingColor, glowingBrighterColor);
  circle.setPosition(center - sf::Vector2f(50, 50));

  target.draw(particles);
  target.draw(circle);
}
void addAlpha(ParticleSystem &particles, sf::RenderTarget &target,
              sf::Clock &clock, const sf::Vector2f &center) {

  for (std::size_t i = 0; i < particles.m_vertices.getVertexCount(); ++i) {
    sf::Vertex &particle = particles.m_vertices[i];
    auto direction = particle.position - center;
    float length =
        std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction /= length;
    particle.position +=
        direction * 2.f; // Slow down the speed by reducing the factor

    // Update the alpha (transparency) of the particle according to its distance
    // from the center
    float ratio = length / std::max(target.getSize().x, target.getSize().y);
    particle.color.a = static_cast<sf::Uint8>(
        (std::sin(ratio * 3.14159f * 2) + 1) / 2 *
        255); // Use a sine function to create a pulsating effect

    // If the particle is out of the screen, reset its position to the center
    if (particle.position.x < 0 || particle.position.x > target.getSize().x ||
        particle.position.y < 0 || particle.position.y > target.getSize().y) {
      particle.position = center;
    }
  }
}
Effects::Effects(sf::RenderTarget &target, sf::Clock &clock, unsigned int count,
                 const std::vector<std::vector<char>> &numbers)
    : target(target), clock(clock), count(count), numbers(numbers) {}

void Effects::next() { current_effect = (current_effect + 1) % 5; }

void Effects::apply(const int effect_type,
                    const std::vector<std::vector<char>> &numbers) {

  switch (effect_type) {
  case 0:
    drawStars(target, clock);
    std::cout << "drawStars" << std::endl;
    break;
  case 1:
    drawGalaxy(target, clock);
    std::cout << "drawGalaxy" << std::endl;
    break;
  case 2:
    MetShower(target, clock);
    std::cout << "MetShower" << std::endl;
    break;
  case 3:
    drawShootingStar(target, clock);
    std::cout << "drawShootingStar" << std::endl;
    break;
  case 4:
    drawTunnel(target, clock);
    std::cout << "drawTunnel" << std::endl;
    break;
  case 5:
    drawColoredSkies(target, clock);
    std::cout << "drawColoredSkies" << std::endl;
    break;
  case 6:
    drawSmoke(target, clock);
    std::cout << "drawSmoke" << std::endl;
    break;
  case 7:
    drawFire(target, clock);
    std::cout << "drawFire" << std::endl;
    break;
  case 8:
    drawRain(target, clock);
    std::cout << "drawRain" << std::endl;
    break;
  case 9:
    drawAllAround(target, clock, 50, numbers);
    std::cout << "drawAllAround" << std::endl;
    break;
  case 10:
    colorsMode(target, clock);
    std::cout << "colorsMode" << std::endl;
    break;
  default:
    break;
  }
}

void Effects::apply(const std::vector<std::vector<char>> &numbers) {
  apply(current_effect, numbers);
}
