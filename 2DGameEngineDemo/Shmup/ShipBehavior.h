#pragma once
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "AComponent.h"
#include "TransformComponent.h"
#include "ICollisionEvent.h"

// Which keys move this ship, and whether it also reacts to touch/mouse -
// solo mode (the default-constructed scheme) is arrow keys plus touch/mouse,
// matching the game's original single-ship behavior exactly. 2-player co-op
// gives each ship its own keyboard-only scheme instead (see ShmupScene::init()):
// touch/mouse must be disabled for both there, since TouchInput and
// sf::Mouse are both global singletons - two ships reading them would both
// react identically to the same one finger/cursor.
struct ShipInputScheme
{
	sf::Keyboard::Scan up = sf::Keyboard::Scan::Up;
	sf::Keyboard::Scan down = sf::Keyboard::Scan::Down;
	sf::Keyboard::Scan left = sf::Keyboard::Scan::Left;
	sf::Keyboard::Scan right = sf::Keyboard::Scan::Right;
	// Solo-only alternate keyset, checked alongside up/down/left/right (see
	// ShipBehavior::update()) rather than instead of it, so the player can
	// freely mix either layout. Scan::W/A/S/D are the same physical keys
	// 2-player co-op's player 0 already uses (see ShmupScene::init(), whose
	// on-screen hint labels them "Z","Q","S","D" - the printed letters at
	// those positions on an AZERTY keyboard, which scancodes name after
	// their QWERTY position). Only ever read when enableTouchAndMouse is
	// true (solo's own default-constructed scheme) - each of 2-player
	// co-op's two schemes must stay exclusive to its own keys, so these are
	// simply never checked there, regardless of what they're left at.
	sf::Keyboard::Scan altUp = sf::Keyboard::Scan::W;
	sf::Keyboard::Scan altDown = sf::Keyboard::Scan::S;
	sf::Keyboard::Scan altLeft = sf::Keyboard::Scan::A;
	sf::Keyboard::Scan altRight = sf::Keyboard::Scan::D;
	bool enableTouchAndMouse = true;
};

// Player ship: dragged by the touch/mouse position - moves by exactly how
// far the finger/cursor moved since last frame while held down, not toward
// its absolute position (see update()) - or a configurable set of keys
// (direct velocity), and fires automatically on a fixed timer. There is no
// manual fire input by design: the player only ever manages movement (held
// touch on mobile, held left mouse button or keys on desktop).
class ShipBehavior : public AComponent, public ICollisionEvent
{
	TransformComponent* transformComp = nullptr;

	// Which player this is (0/1) and which keys/pointer sources drive it -
	// see ShipInputScheme above. playerId doesn't affect movement itself
	// today; it exists so other per-ship systems (individual scoring, a
	// later stage) can tell the two ships apart.
	int playerId = 0;
	ShipInputScheme inputScheme;

	// The actual play area (see ShmupScene::init() - NOT necessarily the
	// canvas's declared 720x1280: SDL reports whatever size CSS actually
	// renders the canvas at). Everything below is derived from this at
	// init() time instead of being a hardcoded pixel constant, so the touch
	// target clamp always agrees with the coordinate space TouchInput
	// reports positions in.
	float playAreaWidth = 0.f;
	float playAreaHeight = 0.f;
	static constexpr float shipHalfSizeFraction = 0.05f;

	float moveSpeed = 0.f;

	// Drives the drag-delta movement (see update()): whether the
	// touch/mouse was already down last frame, and where it was then - a
	// fresh press (wasDown transitioning false->true) deliberately produces
	// zero delta on its first frame, so touching down far from the ship
	// without moving the finger doesn't yank it toward that point.
	bool wasDown = false;
	sf::Vec2f lastRawPosition;

	sf::Clock fireClock;
	float fireInterval = 0.25f;
	// fireInterval never drops below this from FasterFire power-ups (see
	// applyFasterFirePowerUp()) - a floor instead of no limit, so stacking
	// several keeps helping but can't turn the ship into an unbeatable
	// laser.
	static constexpr float minFireInterval = 0.08f;
	static constexpr float fasterFireMultiplier = 0.8f;
	float bulletSpeed = 0.f;
	// 3 hits to kill a regular enemy - see ShmupEnemyBehavior's enemyHp.
	float bulletDamage = 20.f;

	// MultiShot power-up (see applyMultiShotPowerUp()/fireBullet()) - unlike
	// FasterFire's permanent fireInterval reduction, this expires: picking
	// one up (re)starts the timer rather than stacking duration.
	bool multiShotActive = false;
	float multiShotTimer = 0.f;
	static constexpr float multiShotDuration = 10.f;
	// Half-angle of the two side shots around straight up, in radians
	// (~17 degrees) - wide enough to visibly read as a spread, not so wide
	// the side shots immediately fly off past the play area's edges.
	static constexpr float multiShotSpreadAngle = 0.3f;

	// Shield power-up (see applyShieldPowerUp()) - no timer: stays active
	// indefinitely until it actually blocks a hit (bullet or collision),
	// then breaks. The actual damage immunity lives on this ship's own
	// AliveComponent (see AliveComponent::setInvulnerable()/takeDamage()'s
	// consume-on-hit behavior); shieldActive here just mirrors that to drive
	// ShieldEffect's visual bubble via isShieldActive() - see update(),
	// which polls isInvulnerable() every frame to notice when it's consumed.
	bool shieldActive = false;

	// Heal power-up (see applyHealPowerUp()) - instant, no state to track
	// here beyond the fixed amount restored.
	static constexpr float healPowerUpAmount = 35.f;

	bool movedOnce = false;

public:
	// _playerId/_inputScheme default to solo, single-ship behavior (arrow
	// keys + touch/mouse, player 0) - the existing single-ship call site
	// keeps compiling and behaving identically without passing them.
	void init(float _playAreaWidth, float _playAreaHeight, int _playerId = 0, ShipInputScheme _inputScheme = {});
	void update(float _deltaTime) override;
	void beginCollision(ACollider* _me, ACollider* _other, b2Vec2 _normal) override;
	void endCollision(ACollider* _me, ACollider* _other) override;
	// Used by ShmupControlHint to hide itself once the player has proven
	// they've found a way to move, regardless of which input method
	// (keyboard, mouse, or touch) they actually used.
	bool hasMovedOnce() const { return movedOnce; }
	// Used by CoinPickup to credit whichever ship intercepts a coin.
	int getPlayerId() const { return playerId; }

	// Called by PowerUpPickup when this ship intercepts one - see each
	// method's own field comments above for exactly what changes.
	// FasterFire is permanent (lasts the rest of the run); MultiShot expires
	// on its own timer; Shield stays active until it blocks a hit; Heal is
	// instant.
	void applyFasterFirePowerUp();
	void applyMultiShotPowerUp();
	void applyShieldPowerUp();
	void applyHealPowerUp();
	// Used by ShieldEffect to know whether to keep drawing/following this
	// ship, and by applyShieldPowerUp() to avoid spawning a second bubble
	// entity when a Shield pickup is caught while one is already active.
	bool isShieldActive() const { return shieldActive; }

private:
	void fireBullet();
};
