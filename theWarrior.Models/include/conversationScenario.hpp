#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>
#include <boost/variant.hpp>
#include <string>
#include <vector>

namespace thewarrior::models {

using ConversationNodeId = std::string;
using ConversationScenarioId = std::string;
using StoryId = std::string;
using ItemId = std::string;

enum class ConversationNodeTransitionType {
    NextInOrder,
    Stop,
    SpecificNode
};

class ConversationNodeTransition {
 public:
    ConversationNodeTransition() = default;
    static ConversationNodeTransition nextInOrder();
    static ConversationNodeTransition stop();
    static ConversationNodeTransition toNode(const ConversationNodeId &nodeId);
    ConversationNodeTransitionType getType() const;
    const ConversationNodeId &getNextNodeId() const;

 private:
    friend class boost::serialization::access;
    ConversationNodeTransitionType m_type =
        ConversationNodeTransitionType::NextInOrder;
    ConversationNodeId m_nextNodeId;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & m_type;
        ar & m_nextNodeId;
    }
};

struct ConversationDialogue {
    std::vector<std::string> lines;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & lines;
    }
};

struct ConversationChoiceOption {
    std::string text;
    ConversationNodeId nextNodeId;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & text;
        ar & nextNodeId;
    }
};

struct ConversationChoice {
    std::string prompt;
    std::vector<ConversationChoiceOption> options;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & prompt;
        ar & options;
    }
};

// Actions

struct GoldReward {
    unsigned int amount = 0;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & amount;
    }
};

struct ItemReward {
    ItemId itemId;
    unsigned int quantity = 1;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & itemId;
        ar & quantity;
    }
};

using Reward = boost::variant<GoldReward, ItemReward>;

struct RewardAction {
    Reward reward;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & reward;
    }
};

struct ProgressStoryLineAction {
    StoryId storyId;
    std::string stageId;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & storyId;
        ar & stageId;
    }
};

struct RestRequestedAction {
    unsigned int goldCost = 0;
    bool restoreHealth = true;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & goldCost;
        ar & restoreHealth;
    }
};

using ConversationAction = boost::variant<
    RewardAction,
    ProgressStoryLineAction,
    RestRequestedAction
>;

// Conversations

using ConversationNodeContent = boost::variant<
    ConversationDialogue,
    ConversationChoice,
    ConversationAction
>;

class ConversationNode {
 public:
    ConversationNode() = default;
    ConversationNode(const ConversationNodeId &id,
                     const ConversationNodeContent &content,
                     const ConversationNodeTransition &transition =
                         ConversationNodeTransition::nextInOrder());
    static std::string getContentTypeName(const ConversationNodeContent &content);
    const ConversationNodeId &getId() const;
    const ConversationNodeContent &getContent() const;
    const ConversationNodeTransition &getTransition() const;
    void setId(const ConversationNodeId &id);
    void setContent(const ConversationNodeContent &content);
    void setTransition(const ConversationNodeTransition &transition);

 private:
    friend class boost::serialization::access;
    ConversationNodeId m_id;
    ConversationNodeContent m_content;
    ConversationNodeTransition m_transition;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & m_id;
        ar & m_content;
        if (version >= 1) {
            ar & m_transition;
        } else {
            m_transition = ConversationNodeTransition::nextInOrder();
        }
    }
};

class ConversationScenario {
 public:
    ConversationScenario() = default;
    ConversationScenario(const ConversationScenarioId &id,
                         const StoryId &conditionalStoryIdCompleted,
                         const std::vector<ConversationNode> &nodes);
    static ConversationScenario fromLegacyDialogueLines(
        const std::vector<std::string> &lines);
    const ConversationScenarioId &getId() const;
    const StoryId &getConditionalStoryIdCompleted() const;
    const std::vector<ConversationNode> &getNodes() const;
    std::vector<std::string> getDialogueLines() const;
    void setId(const ConversationScenarioId &id);
    void setConditionalStoryIdCompleted(const StoryId &storyId);
    void setNodes(const std::vector<ConversationNode> &nodes);
    void setDialogueLines(const std::vector<std::string> &lines);
    void addNode(const ConversationNode &node);
    bool replaceNode(const ConversationNodeId &id,
                     const ConversationNode &node);
    bool removeNode(const ConversationNodeId &id);

 private:
    friend class boost::serialization::access;
    ConversationScenarioId m_id;
    StoryId m_conditionalStoryIdCompleted;
    std::vector<ConversationNode> m_nodes;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & m_id;
        ar & m_conditionalStoryIdCompleted;
        ar & m_nodes;
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::ConversationNode, 1)
