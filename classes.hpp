#include <iostream>
#include <memory>
#include <map>
#include <string>

// base class
class ExprNode
{
public:
    // virtual eval that is implented differently in various derived classes
    virtual double eval() = 0;
    // virtual derivative function to implement in derived classes
    virtual std::shared_ptr<ExprNode> derivative(std::string var) = 0;
    // overloaded << operation, that will call the virtual print in the derived classes
    friend std::ostream &operator<<(std::ostream &os, const ExprNode &node)
    {
        return node.print(os);
    }
    virtual std::ostream &print(std::ostream &os) const = 0;
};

using node_ptr = std::shared_ptr<ExprNode>;

// leaf node - Constant
class ConstantNode : public ExprNode
{
public:
    ConstantNode(double val) : value(val) {}
    // just return the value
    double eval() override { return value; }

    node_ptr derivative(std::string var) override { return std::make_shared<ConstantNode>(0.0); }

    std::ostream &print(std::ostream &os) const override { return os << value; }

private:
    double value;
};

// leaf node - variables
class VariableNode : public ExprNode
{
public:
    VariableNode(std::string name) : name(name) {}
    double eval() override
    {
        // find the value and store it in x
        auto x = symbol_table.find(name);

        // return the value from the map
        return x->second;
    }

    // 1 if its the variable, otherwise 0
    node_ptr derivative(std::string var) override
    {
        if (var == name)
        {
            return std::make_shared<ConstantNode>(1.0);
        }
        else
        {
            return std::make_shared<ConstantNode>(0.0);
        }
    }

    static void set_value(std::string name, double value)
    {
        symbol_table[name] = value;
    }
    std::ostream &print(std::ostream &os) const override { return os << name; }
    static std::map<std::string, double> symbol_table;

private:
    std::string name;
};

// make the symbol table available to all instances of variable class
std::map<std::string, double> VariableNode::symbol_table;

class AddNode : public ExprNode
{
public:
    AddNode(node_ptr newLeft, node_ptr newRight)
        : left(newLeft), right(newRight) {}
    double eval() override
    {
        return left->eval() + right->eval();
    }

    // sum of the derived
    node_ptr derivative(std::string var) override
    {
        return std::make_shared<AddNode>(left->derivative(var), right->derivative(var));
    }

    std::ostream &print(std::ostream &os) const override { return os << "(" << *left << "+" << *right << ")"; }

private:
    node_ptr left, right;
};

class SubNode : public ExprNode
{
public:
    SubNode(node_ptr newLeft, node_ptr newRight)
        : left(newLeft), right(newRight) {}
    double eval() override
    {
        return left->eval() - right->eval();
    }

    node_ptr derivative(std::string var) override
    {
        return std::make_shared<SubNode>(left->derivative(var), right->derivative(var));
    }

    std::ostream &print(std::ostream &os) const override { return os << "(" << *left << "-" << *right << ")"; }

private:
    node_ptr left, right;
};

class MulNode : public ExprNode
{
public:
    MulNode(node_ptr newLeft, node_ptr newRight)
        : left(newLeft), right(newRight) {}
    double eval() override
    {
        return left->eval() * right->eval();
    }

    node_ptr derivative(std::string variable) override
    {
        // Apply the product rule: (u * v)' = u' * v + u * v'
        return std::make_shared<AddNode>(
            std::make_shared<MulNode>(left->derivative(variable), right),
            std::make_shared<MulNode>(left, right->derivative(variable)));
    }

    std::ostream &print(std::ostream &os) const override { return os << "(" << *left << "*" << *right << ")"; }

private:
    node_ptr left, right;
};

class DivNode : public ExprNode
{
public:
    DivNode(node_ptr newLeft, node_ptr newRight)
        : left(newLeft), right(newRight) {}
    double eval() override
    {
        double denominator = right->eval();
        return left->eval() / denominator;
    }

    node_ptr derivative(std::string variable) override
    {
        // Apply the quotient rule: (u / v)' = (u'v - uv') / v^2
        return std::make_shared<DivNode>(
            std::make_shared<SubNode>(
                std::make_shared<MulNode>(left->derivative(variable), right),
                std::make_shared<MulNode>(left, right->derivative(variable))),
            std::make_shared<MulNode>(right, right));
    }

    std::ostream &print(std::ostream &os) const override { return os << "(" << *left << "*" << *right << ")"; }

private:
    node_ptr left, right;
};
