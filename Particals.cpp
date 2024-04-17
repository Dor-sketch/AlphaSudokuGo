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
                               unsigned int windowHeight)
    : m_particles(count),
      m_vertices(sf::Lines, count * 2), // two vertices per particle
      m_lifetime(sf::seconds(3)), m_emitter(0, 0), m_windowWidth(windowWidth),
      m_windowHeight(windowHeight){};

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
    float speed = (std::rand() % 10) + 5.f; // increased speed
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
  float speed = (std::rand() % 2) + 4.f; // increased speed for larger "hall"
  m_particles[index].velocity =
      sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);

  // calculate the maximum distance a particle could need to travel (diagonal
  // across the screen)
  float maxDistance = std::sqrt(m_windowWidth * m_windowWidth +
                                m_windowHeight * m_windowHeight);

  // calculate the time it would take for the particle to travel that distance
  // at its current speed
  sf::Time maxLifetime = sf::seconds(maxDistance / speed);

  // set the particle's lifetime to a random value between 1000 milliseconds and
  // the calculated maximum
  m_particles[index].lifetime = sf::milliseconds(
      (std::rand() % static_cast<int>(maxLifetime.asMilliseconds())) + 1000);

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

void ParticleSystem::changeColor(sf::Color color) {
  for (std::size_t i = 0; i < m_particles.size(); ++i) {
    m_vertices[i * 2].color = color;
    m_vertices[i * 2 + 1].color = color;
  }
}

DotsParticleSystem::DotsParticleSystem(unsigned int count,
                                       unsigned int windowWidth,
                                       unsigned int windowHeight)
    : ParticleSystem(count, windowWidth,
                     windowHeight), // Call the ParticleSystem constructor
      m_vertices(sf::Points, count), m_lifetime(sf::seconds(3)),
      m_emitter(0, 0) {
  // Initialize m_windowWidth and m_windowHeight
  this->m_windowWidth = windowWidth;
  this->m_windowHeight = windowHeight;
}

void DotsParticleSystem::spawnParticles(unsigned int count) {
  for (unsigned int i = 0; i < count; ++i) {
    resetParticle(i);
  }
}
void DotsParticleSystem::setEmitter(sf::Vector2f position) {
  m_emitter = position;
}

void DotsParticleSystem::update(sf::Time elapsed) {
  for (std::size_t i = 0; i < m_particles.size(); ++i) {
    // update the particle lifetime
    Particle &p = m_particles[i];
    p.lifetime -= elapsed;

    // if the particle is dead, respawn it
    if (p.lifetime <= sf::Time::Zero)
      resetParticle(i);

    // update the position of the corresponding vertex
    m_vertices[i].position += p.velocity * elapsed.asSeconds();

    // update the alpha (transparency) of the particle according to its lifetime
    float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
    m_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
  }
}
void DotsParticleSystem::draw(sf::RenderTarget &target,
                              sf::RenderStates states) const {
  states.transform *= getTransform();
  states.texture = NULL;
  target.draw(m_vertices, states);
}

void DotsParticleSystem::resetParticle(std::size_t index) {
  // give a random velocity and lifetime to the particle
  float angle = (std::rand() % 360) * 3.14159f / 180;
  float speed = (std::rand() % 3) + 50.f;
  m_particles[index].velocity =
      sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
  m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);
  m_vertices[index].position =
      sf::Vector2f(std::rand() % m_windowWidth, std::rand() % m_windowHeight);
}

void DotsParticleSystem::setMovingDirectionEffect(const sf::Vector2f &center) {
  for (std::size_t i = 0; i < m_particles.size(); ++i) {
    sf::Vector2f direction = m_vertices[i].position - center;
    float length =
        std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length != 0) {     // avoid division by zero
      direction /= length; // normalize the direction vector
    }
    float speed = (std::rand() % 50) + 50.f; // random speed
    m_particles[i].velocity = direction * speed;
  }
}

void DotsParticleSystem::setMovingDirectionEffect() {
  for (std::size_t i = 0; i < m_particles.size(); ++i) {
    float angle = (std::rand() % 360) * 3.14159f / 180;
    float speed = (std::rand() % 50) + 50.f;
    m_particles[i].velocity =
        sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
  }
}

void DotsParticleSystem::changeColor(sf::Color color) {
  for (std::size_t i = 0; i < m_particles.size(); ++i) {
    m_vertices[i].color = color;
  }
}

void SmokeParticleSystem::spawnParticles(unsigned int count) {
  for (unsigned int i = 0; i < count; ++i) {
    resetParticle(i);
  }
}

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

SmokeParticleSystem::SmokeParticleSystem(unsigned int count,
                                         unsigned int windowWidth,
                                         unsigned int windowHeight)
    : ParticleSystem(count, windowWidth,
                     windowHeight), // Call the ParticleSystem constructor
      m_vertices(sf::Points, count), m_lifetime(sf::seconds(3)),
      m_emitter(0, 0) {
  // Initialize m_windowWidth and m_windowHeight
  this->m_windowWidth = windowWidth;
  this->m_windowHeight = windowHeight;
}

void SmokeParticleSystem::setEmitter(sf::Vector2f position) {
  m_emitter = position;
}

void SmokeParticleSystem::update(sf::Time elapsed) {
  for (std::size_t i = 0; i < m_particles.size(); ++i) {
    // update the particle lifetime
    Particle &p = m_particles[i];
    p.lifetime -= elapsed;

    // if the particle is dead, respawn it
    if (p.lifetime <= sf::Time::Zero)
      resetParticle(i);

    // update the position of the corresponding vertex
    m_vertices[i].position += p.velocity * elapsed.asSeconds();

    // update the alpha (transparency) of the particle according to its
    // lifetime
    float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
    m_vertices[i].color.a = static_cast<sf::Uint8>(
        ratio * count / 4); // big number means bigger sphere
  }
}
void SmokeParticleSystem::draw(sf::RenderTarget &target,
                               sf::RenderStates states) const {
  states.transform *= getTransform();
  states.texture = NULL;
  target.draw(m_vertices, states);
}
void SmokeParticleSystem::setMovingDirectionEffect(const sf::Vector2f &center) {
  for (std::size_t i = 0; i < m_particles.size(); ++i) {
    sf::Vector2f direction = m_vertices[i].position - center;
    float length =
        std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length != 0) {     // avoid division by zero
      direction /= length; // normalize the direction vector
    }
    float speed = (std::rand() % 50) + 50.f; // random speed
    m_particles[i].velocity = direction * speed;
  }
}

void SmokeParticleSystem::setMovingDirectionEffect() {
  for (std::size_t i = 0; i < m_particles.size(); ++i) {
    float angle = (std::rand() % 360) * 3.14159f / 180;
    float speed = (std::rand() % 50) + 50.f;
    m_particles[i].velocity =
        sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
  }
}

void SmokeParticleSystem::changeColor(sf::Color color) {
  for (std::size_t i = 0; i < m_particles.size(); ++i) {
    m_vertices[i].color = color;
  }
}

void SmokeParticleSystem::resetParticle(std::size_t index) {
  // give a random velocity and lifetime to the particle
  float angle = (std::rand() % 360) * 3.14f / 180.f;
  float speed = (std::rand() % 50) + 50.f;
  m_particles[index].velocity =
      sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
  m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);

  // reset the position of the corresponding vertex
  m_vertices[index].position = m_emitter;
}

void drawMovingBackgroundSpace(sf::RenderTarget &target, sf::Clock &clock,
                               unsigned int count,
                               const std::vector<std::vector<char>> &numbers) {
  static sf::Vector2f direction(0, 0);
  direction.x = std::sin(clock.getElapsedTime().asSeconds());
  direction.y = std::cos(clock.getElapsedTime().asSeconds());
  unsigned int cellsDrawn = howManyCellsDrawn(numbers);
  static ParticleSystem particles(cellsDrawn * count, target.getSize().x,
                                  target.getSize().y);
  // add glow effect to the particles
  auto glowingColor = GlowingColor(analogousCyan).getShade();
  auto glowingBrighterColor = GlowingColor(analogousCyan).getBrighterShade();
  for (std::size_t i = 0; i < particles.m_particles.size(); ++i) {
    particles.m_vertices[i * 2].color = glowingColor;
    particles.m_vertices[i * 2 + 1].color = glowingBrighterColor;
  }

  // calculate the offset based on a sine function of time
  float offsetMagnitude =
      50.0f; // adjust this value to change the magnitude of the offset
  sf::Vector2f offset(
      offsetMagnitude * std::sin(clock.getElapsedTime().asSeconds()), 0);

  // add the offset to the center position
  sf::Vector2f center(target.getSize().x / 2 + offset.x,
                      target.getSize().y / 2 + offset.y);

  particles.setEmitter(center);
  particles.update(clock.restart());

  // draw a small black circle at the center position
  sf::CircleShape blackCenter(
      25); // adjust this value to change the size of the black center
  blackCenter.setFillColor(sf::Color::Black);
  blackCenter.setPosition(
      center - sf::Vector2f(25, 25)); // adjust the position so the center of
                                      // the circle is at the center position
  // particles.setMovingDirectionEffect(center);

  target.draw(particles);
  target.draw(blackCenter); // draw the black center after the particles so it
                            // appears on top
}

void drawMovingBackgroundSmoke(sf::RenderTarget &target, sf::Clock &clock,
                               unsigned int count,
                               const std::vector<std::vector<char>> &numbers) {
  static sf::Vector2f direction(0, 0);
  direction.x = std::sin(clock.getElapsedTime().asSeconds());
  direction.y = std::cos(clock.getElapsedTime().asSeconds());
  unsigned int cellsDrawn = howManyCellsDrawn(numbers);
  static SmokeParticleSystem particles(cellsDrawn * count, target.getSize().x,
                                       target.getSize().y);
  // add glow effect to the particles
  auto glowingColor = GlowingColor(analogousCyan).getShade();
  auto glowingBrighterColor = GlowingColor(analogousCyan).getBrighterShade();
  for (std::size_t i = 0; i < particles.m_particles.size(); ++i) {
    particles.m_vertices[i * 2].color = glowingColor;
    particles.m_vertices[i * 2 + 1].color = glowingBrighterColor;
  }

  // calculate the offset based on a sine function of time
  float offsetMagnitude =
      50.0f; // adjust this value to change the magnitude of the offset
  sf::Vector2f offset(
      offsetMagnitude * std::sin(clock.getElapsedTime().asSeconds()), 0);

  // add the offset to the center position
  sf::Vector2f center(target.getSize().x / 2 + offset.x,
                      target.getSize().y / 2 + offset.y);

  particles.setEmitter(center);
  particles.update(clock.restart());

  // draw a small black circle at the center position
  sf::CircleShape blackCenter(
      25); // adjust this value to change the size of the black center
  blackCenter.setFillColor(sf::Color::Black);
  blackCenter.setPosition(
      center - sf::Vector2f(25, 25)); // adjust the position so the center of
                                      // the circle is at the center position
  // particles.setMovingDirectionEffect(center);

  target.draw(particles);
  target.draw(blackCenter); // draw the black center after the particles so it
                            // appears on top
}

void drawAllAround(sf::RenderTarget &target, sf::Clock clock,
                   unsigned int count,
                   const std::vector<std::vector<char>> &numbers) {
  //     static ParticleSystem particles(100, target.getSize().x,
  //     target.getSize().y); static sf::Clock spawnClock;

  //     // // Spawn new particles every 100 milliseconds
  //     // if (spawnClock.getElapsedTime() > sf::milliseconds(1000)) {
  //     //     particles.spawnParticles(10); // Spawn 10 new particles
  //     //     spawnClock.restart();
  //     // }

  //     particles.update(clock.restart());
  //         particles.setMovingDirectionEffect(sf::Vector2f(target.getSize().x
  //         / 2, target.getSize().y / 2));
  // particles. setEmitter(sf::Vector2f(target.getSize().x / 4,
  // target.getSize().y / 4));
  //     target.draw(particles);

  static DotsParticleSystem dots(100000, target.getSize().x,
                                 target.getSize().y);
  static sf::Clock dotsSpawnClock;
  std::cout << "dotsSpawnClock.getElapsedTime() "
            << dotsSpawnClock.getElapsedTime().asMilliseconds() << std::endl;

  // Spawn new particles every 100 milliseconds
  if (dotsSpawnClock.getElapsedTime() > sf::milliseconds(100)) {
    dots.spawnParticles(10); // Spawn 10 new particles
    dotsSpawnClock.restart();
  }

  dots.update(clock.restart());
  dots.setEmitter(sf::Vector2f(target.getSize().x / 2, target.getSize().y / 2));
  auto direction = sf::Vector2f(target.getSize().x / 2, target.getSize().y / 2);
  dots.setMovingDirectionEffect(direction);
  static GlowingColor glowingColor(analogousCyan);
  dots.changeColor(glowingColor.getShade());

  // target.draw(dots);

  static SmokeParticleSystem smoke(1000, target.getSize().x,
                                   target.getSize().y);
  static sf::Clock smokeSpawnClock;
  smoke.setEmitter(
      sf::Vector2f(target.getSize().x / 2, target.getSize().y / 2));
  // Spawn new particles every 100 milliseconds
  if (smokeSpawnClock.getElapsedTime() > sf::milliseconds(100)) {
    smoke.spawnParticles(10); // Spawn 10 new particles
    smokeSpawnClock.restart();
  }

  smoke.update(clock.restart());
  target.draw(smoke);
}

void drawSmoke(sf::RenderTarget &target, sf::Clock &clock) {
  static ParticleSystem particles(1000, target.getSize().x, target.getSize().y);
  static sf::Clock spawnClock;

  // Spawn new particles every 100 milliseconds
  if (spawnClock.getElapsedTime() > sf::milliseconds(100)) {
    particles.spawnParticles(10); // Spawn 10 new particles
    spawnClock.restart();
  }

  particles.update(clock.restart());
  target.draw(particles);
}
void draw_creativity(sf::RenderTarget &target, sf::Clock &clock,
                     unsigned int count,
                     const std::vector<std::vector<char>> &numbers) {
  static std::vector<std::unique_ptr<ParticleSystem>> particleSystems;
  static sf::Clock spawnClock;
  static sf::Clock changeClock;

  // Initialize particle systems// Initialize particle systems
  // Initialize particle systems
  if (particleSystems.empty()) {
    for (int i = 0; i < 5; ++i) {
      int type = rand() % 3; // Assuming there are 3 types of particle systems
      switch (type) {
      case 0:
        particleSystems.push_back(
            std::unique_ptr<ParticleSystem>(new SmokeParticleSystem(
                200 * (i + 1), target.getSize().x, target.getSize().y)));
        break;
      case 1:
        particleSystems.push_back(std::make_unique<ParticleSystem>(
            200 * (i + 1), target.getSize().x, target.getSize().y));
        break;
      case 2:
        particleSystems.push_back(
            std::unique_ptr<ParticleSystem>(new DotsParticleSystem(
                200 * (i + 1), target.getSize().x, target.getSize().y)));
        break;
      }
    }
  }
  // Spawn new particles every 100 milliseconds
  if (spawnClock.getElapsedTime() > sf::milliseconds(100)) {
    for (auto &particles : particleSystems) {
      particles->spawnParticles(
          rand() % 20 + 1); // Spawn a random number of new particles per system
    }
    spawnClock.restart();
  }

  // Change particle properties every second
  if (changeClock.getElapsedTime() > sf::seconds(1)) {
    for (auto &particles : particleSystems) {
      // Generate random color
      sf::Color color(rand() % 256, rand() % 256, rand() % 256);

      // Generate random direction
      sf::Vector2f direction(rand() % 100 - 50, rand() % 100 - 50);

      particles->changeColor(color);
      particles->setMovingDirectionEffect(direction);
    }
    changeClock.restart();
  }

  for (auto &particles : particleSystems) {
    particles->update(clock.restart());
    target.draw(*particles);
  }
}
Effects::Effects(sf::RenderTarget &target, sf::Clock &clock, unsigned int count,
                 const std::vector<std::vector<char>> &numbers)
    : target(target), clock(clock), count(count), numbers(numbers) {}

void Effects::apply(const int effect_type,
                    const std::vector<std::vector<char>> &numbers) {
  switch (effect_type) {
  case 0:
    drawMovingBackgroundSpace(target, clock, count, numbers);
    break;
  case 1:
    drawMovingBackgroundSmoke(target, clock, count, numbers);
    break;
  case 2:
    drawAllAround(target, clock, count, numbers);
    break;
  default:
    draw_creativity(target, clock, count, numbers);
    break;
  }
}
