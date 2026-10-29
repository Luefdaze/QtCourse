#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QKeyEvent>
#include <QLineEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QtMath>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->display->setReadOnly(true);
    ui->display->setFocusPolicy(Qt::NoFocus);
    ui->display->setText(QStringLiteral("0"));
    setFocusPolicy(Qt::StrongFocus);

    const auto buttons = findChildren<QPushButton *>();
    for (QPushButton *button : buttons) {
        button->setFocusPolicy(Qt::NoFocus);
        connect(button, &QPushButton::clicked, this, &MainWindow::btn_Num_Clicked);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::btn_Num_Clicked()
{
    const auto *button = qobject_cast<QPushButton *>(sender());
    if (button)
        handleInput(button->text());
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        handleInput(QString::number(key - Qt::Key_0));
    } else {
        switch (key) {
        case Qt::Key_Period:
        case Qt::Key_Comma:
            handleInput(QStringLiteral("."));
            break;
        case Qt::Key_Plus:
            handleInput(QStringLiteral("+"));
            break;
        case Qt::Key_Minus:
            handleInput(QStringLiteral("-"));
            break;
        case Qt::Key_Asterisk:
        case Qt::Key_X:
            handleInput(QStringLiteral("*"));
            break;
        case Qt::Key_Slash:
            handleInput(QStringLiteral("/"));
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Equal:
            handleInput(QStringLiteral("="));
            break;
        case Qt::Key_Backspace:
            handleInput(QStringLiteral("backspace"));
            break;
        case Qt::Key_Delete:
            handleInput(QStringLiteral("CE"));
            break;
        case Qt::Key_Escape:
            handleInput(QStringLiteral("C"));
            break;
        case Qt::Key_Percent:
            handleInput(QStringLiteral("%"));
            break;
        default:
            QMainWindow::keyPressEvent(event);
            return;
        }
    }
    event->accept();
}

void MainWindow::handleInput(const QString &input)
{
    if (input.size() == 1 && input.at(0).isDigit()) {
        inputDigit(input.at(0));
        return;
    }

    if (input == QStringLiteral(".")) {
        inputDecimalPoint();
    } else if (input == QStringLiteral("+")) {
        inputOperator(QLatin1Char('+'));
    } else if (input == QStringLiteral("-")) {
        inputOperator(QLatin1Char('-'));
    } else if (input == QStringLiteral("*") || input == QStringLiteral("\u00D7")) {
        inputOperator(QLatin1Char('*'));
    } else if (input == QStringLiteral("/") || input == QStringLiteral("\u00F7")) {
        inputOperator(QLatin1Char('/'));
    } else if (input == QStringLiteral("=")) {
        calculateResult();
    } else if (input == QStringLiteral("backspace") || input == QStringLiteral("\u2190")
               || input == QStringLiteral("\u232B") || input == QStringLiteral("\U0001F519")) {
        backspace();
    } else if (input == QStringLiteral("C")) {
        clearAll();
    } else if (input == QStringLiteral("CE")) {
        clearEntry();
    } else if (input == QStringLiteral("+/-")) {
        applyUnaryOperation(QStringLiteral("sign"));
    } else if (input == QStringLiteral("%")) {
        applyUnaryOperation(QStringLiteral("percent"));
    } else if (input == QStringLiteral("\u221Ax")) {
        applyUnaryOperation(QStringLiteral("sqrt"));
    } else if (input == QStringLiteral("x\u00B2")) {
        applyUnaryOperation(QStringLiteral("square"));
    } else if (input == QStringLiteral("1/x")) {
        applyUnaryOperation(QStringLiteral("reciprocal"));
    }
}

void MainWindow::inputDigit(const QChar &digit)
{
    if (m_hasError || (m_justEvaluated && m_pendingOperator.isNull()))
        clearAll();

    QString text = ui->display->text();
    if (m_waitingForOperand || text == QStringLiteral("0")) {
        text = digit;
        m_waitingForOperand = false;
    } else if (text.remove(QLatin1Char('-')).size() < 15) {
        text = ui->display->text() + digit;
    }

    ui->display->setText(text);
    m_justEvaluated = false;
}

void MainWindow::inputDecimalPoint()
{
    if (m_hasError || (m_justEvaluated && m_pendingOperator.isNull()))
        clearAll();

    if (m_waitingForOperand) {
        ui->display->setText(QStringLiteral("0."));
        m_waitingForOperand = false;
    } else if (!ui->display->text().contains(QLatin1Char('.'))) {
        ui->display->setText(ui->display->text() + QLatin1Char('.'));
    }
    m_justEvaluated = false;
}

void MainWindow::inputOperator(const QChar &op)
{
    if (m_hasError)
        return;

    if (!m_pendingOperator.isNull()) {
        if (!m_waitingForOperand) {
            double result = 0.0;
            if (!calculate(m_firstOperand, m_pendingOperator, currentValue(), result)) {
                showError(QStringLiteral("\u9664\u6570\u4E0D\u80FD\u4E3A\u96F6\u6216\u7ED3\u679C\u8D85\u51FA\u8303\u56F4"));
                return;
            }
            m_firstOperand = result;
            ui->display->setText(formatNumber(result));
        }
    } else {
        m_firstOperand = currentValue();
    }

    m_pendingOperator = op;
    m_waitingForOperand = true;
    m_justEvaluated = false;
    updateExpression();
}

void MainWindow::calculateResult()
{
    if (m_hasError)
        return;

    QChar op;
    double right = 0.0;
    if (!m_pendingOperator.isNull()) {
        op = m_pendingOperator;
        right = m_waitingForOperand ? m_firstOperand : currentValue();
        m_lastOperator = op;
        m_lastOperand = right;
    } else if (m_justEvaluated && !m_lastOperator.isNull()) {
        op = m_lastOperator;
        right = m_lastOperand;
        m_firstOperand = currentValue();
    } else {
        return;
    }

    const double left = m_firstOperand;
    double result = 0.0;
    if (!calculate(m_firstOperand, op, right, result)) {
        showError(QStringLiteral("\u9664\u6570\u4E0D\u80FD\u4E3A\u96F6\u6216\u7ED3\u679C\u8D85\u51FA\u8303\u56F4"));
        return;
    }

    ui->display->setText(formatNumber(result));
    ui->expressionLabel->setText(formatNumber(left) + QStringLiteral(" ")
                                 + displayOperator(op) + QStringLiteral(" ")
                                 + formatNumber(right) + QStringLiteral(" ="));
    m_firstOperand = result;
    m_pendingOperator = QChar();
    m_waitingForOperand = true;
    m_justEvaluated = true;
}

void MainWindow::applyUnaryOperation(const QString &operation)
{
    if (m_hasError)
        return;

    const double value = currentValue();
    double result = value;
    if (operation == QStringLiteral("sign")) {
        result = -value;
    } else if (operation == QStringLiteral("percent")) {
        result = value / 100.0;
    } else if (operation == QStringLiteral("sqrt")) {
        if (value < 0.0) {
            showError(QStringLiteral("\u8D1F\u6570\u4E0D\u80FD\u5F00\u5E73\u65B9"));
            return;
        }
        result = qSqrt(value);
    } else if (operation == QStringLiteral("square")) {
        result = value * value;
    } else if (operation == QStringLiteral("reciprocal")) {
        if (value == 0.0) {
            showError(QStringLiteral("\u96F6\u6CA1\u6709\u5012\u6570"));
            return;
        }
        result = 1.0 / value;
    }

    if (!std::isfinite(result)) {
        showError(QStringLiteral("\u7ED3\u679C\u8D85\u51FA\u8303\u56F4"));
        return;
    }

    ui->display->setText(formatNumber(result));
    m_waitingForOperand = false;
    m_justEvaluated = false;
}

void MainWindow::backspace()
{
    if (m_hasError) {
        clearAll();
        return;
    }
    if (m_waitingForOperand)
        return;

    QString text = ui->display->text();
    text.chop(1);
    if (text.isEmpty() || text == QStringLiteral("-"))
        text = QStringLiteral("0");
    ui->display->setText(text);
    m_justEvaluated = false;
}

void MainWindow::clearEntry()
{
    ui->display->setText(QStringLiteral("0"));
    m_waitingForOperand = true;
    m_justEvaluated = false;
    m_hasError = false;
    if (m_pendingOperator.isNull())
        ui->expressionLabel->clear();
}

void MainWindow::clearAll()
{
    ui->display->setText(QStringLiteral("0"));
    m_firstOperand = 0.0;
    m_lastOperand = 0.0;
    m_pendingOperator = QChar();
    m_lastOperator = QChar();
    m_waitingForOperand = true;
    m_justEvaluated = false;
    m_hasError = false;
    ui->expressionLabel->clear();
    statusBar()->clearMessage();
}

void MainWindow::showError(const QString &message)
{
    ui->display->setText(QStringLiteral("Error"));
    ui->expressionLabel->clear();
    m_pendingOperator = QChar();
    m_lastOperator = QChar();
    m_waitingForOperand = true;
    m_justEvaluated = true;
    m_hasError = true;
    statusBar()->showMessage(message, 4000);
}

void MainWindow::updateExpression()
{
    if (m_pendingOperator.isNull()) {
        ui->expressionLabel->clear();
        return;
    }

    ui->expressionLabel->setText(formatNumber(m_firstOperand) + QStringLiteral(" ")
                                 + displayOperator(m_pendingOperator));
}

QString MainWindow::displayOperator(const QChar &op) const
{
    if (op == QLatin1Char('*'))
        return QStringLiteral("\u00D7");
    if (op == QLatin1Char('/'))
        return QStringLiteral("\u00F7");
    return QString(op);
}

double MainWindow::currentValue() const
{
    return ui->display->text().toDouble();
}

QString MainWindow::formatNumber(double value) const
{
    if (qAbs(value) < 1e-14)
        value = 0.0;
    return QString::number(value, 'g', 15);
}

bool MainWindow::calculate(double left, const QChar &op, double right, double &result) const
{
    switch (op.unicode()) {
    case '+': result = left + right; break;
    case '-': result = left - right; break;
    case '*': result = left * right; break;
    case '/':
        if (right == 0.0)
            return false;
        result = left / right;
        break;
    default:
        return false;
    }
    return std::isfinite(result);
}
