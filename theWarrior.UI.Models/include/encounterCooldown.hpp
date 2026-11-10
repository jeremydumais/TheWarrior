#pragma once

namespace thewarrior::ui::models {

class EncounterCooldown {
 public:
    explicit EncounterCooldown(unsigned int minimumMovements = 5)
        : m_minimumMovements(minimumMovements), m_remainingMovements(minimumMovements) {}

    void reset() {
        m_remainingMovements = m_minimumMovements;
    }

    // Called once per completed tile move, before any encounter roll.
    bool onTileMoveCompleted() {
        if (m_remainingMovements > 0) {
            --m_remainingMovements;
        }
        return m_remainingMovements == 0;
    }

    bool canCheckEncounter() const {
        return m_remainingMovements == 0;
    }

 private:
    unsigned int m_minimumMovements;
    unsigned int m_remainingMovements;
};

}  // namespace thewarrior::ui::models
