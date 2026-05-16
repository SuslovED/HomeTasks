#include <iostream>
#include <utility>

class ASTNode;

class NodeOwner {
    ASTNode *node = nullptr;
public:
    explicit NodeOwner(ASTNode *node) : node(node) {}

    NodeOwner(const NodeOwner &) = delete;
    NodeOwner &operator=(const NodeOwner &) = delete;

    NodeOwner(NodeOwner &&other) noexcept {
        std::swap(node, other.node);
    }

    NodeOwner &operator=(NodeOwner &&other) noexcept {
        std::swap(node, other.node);
        return *this;
    }

    ASTNode *operator->() { return node; }
    const ASTNode *operator->() const { return node; }
    operator bool() const { return node != nullptr; }

    ~NodeOwner();
};

class ASTNode {
public:
    virtual void print() const = 0;
    virtual int get_value() const = 0;
    virtual ~ASTNode() = default;
};

class ASTNumberNode : public ASTNode {
private:
    int value;

    ASTNumberNode(int value) : value(value) {}

public:
    static NodeOwner create(int value) {
        return NodeOwner(new ASTNumberNode(value));
    }

    void print() const override {
        std::cout << value;
    }

    int get_value() const override {
        return value;
    }
};

class ASTPlusNode : public ASTNode {
private:
    NodeOwner left;
    NodeOwner right;

    ASTPlusNode(NodeOwner left, NodeOwner right)
        : left(std::move(left)), right(std::move(right)) {}

public:
    static NodeOwner create(NodeOwner left, NodeOwner right) {
        return NodeOwner(new ASTPlusNode(std::move(left), std::move(right)));
    }

    void print() const override {
        std::cout << "(";
        left->print();
        std::cout << " + ";
        right->print();
        std::cout << ")";
    }

    int get_value() const override {
        return left->get_value() + right->get_value();
    }
};

class ASTMinusNode : public ASTNode {
private:
    NodeOwner left;
    NodeOwner right;

    ASTMinusNode(NodeOwner left, NodeOwner right)
        : left(std::move(left)), right(std::move(right)) {}

public:
    static NodeOwner create(NodeOwner left, NodeOwner right) {
        return NodeOwner(new ASTMinusNode(std::move(left), std::move(right)));
    }

    void print() const override {
        std::cout << "(";
        left->print();
        std::cout << " - ";
        right->print();
        std::cout << ")";
    }

    int get_value() const override {
        return left->get_value() - right->get_value();
    }
};

NodeOwner operator+(NodeOwner left, NodeOwner right) {
    return ASTPlusNode::create(std::move(left), std::move(right));
}

NodeOwner operator-(NodeOwner left, NodeOwner right) {
    return ASTMinusNode::create(std::move(left), std::move(right));
}

NodeOwner::~NodeOwner() {
    if (node) {
        delete node;
    }
}

int main() {
    static_assert(std::is_same<decltype(ASTNumberNode::create(1)), NodeOwner>::value,
                  "ASTNumberNode::create() must return NodeOwner value");
    static_assert(std::is_same<decltype(ASTPlusNode::create(ASTNumberNode::create(1), ASTNumberNode::create(2))), NodeOwner>::value,
                  "ASTPlusNode::create() must return NodeOwner value");
    static_assert(std::is_same<decltype(ASTNumberNode::create(1) + ASTNumberNode::create(2)), NodeOwner>::value,
                  "(NodeOwner + NodeOwner) must return NodeOwner value");

    auto expr_1 = ASTPlusNode::create(
        ASTNumberNode::create(1),
        ASTMinusNode::create(
            ASTNumberNode::create(2),
            ASTNumberNode::create(3)
        )
    );
    expr_1->print();
    std::cout << " = " << expr_1->get_value() << std::endl;

    auto expr_2 = ASTNumberNode::create(4) - ASTNumberNode::create(5) + ASTNumberNode::create(6);
    expr_2->print();
    std::cout << " = " << expr_2->get_value() << std::endl;

    return 0;
}
