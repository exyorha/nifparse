
class ExpressionParser
  TOKENS = [
    [ /[A-Za-z](\s*[A-Za-z0-9\\])*/, :variable ],
    [ /\s+/, :void ],
    [ /!=|==|>=|<=|<<|>>|&&|\|\||[+\-*\/&|><!%\^]/, :operator ],
    [ /\(/, :lparen ],
    [ /\)/, :rparen ],
    [ /[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+/, :version ],
    [ /0x[0-9A-Fa-f]+|0[0-7]+|[0-9]+/, :number ]
  ]

  OPERATORS = {
    :! => [ :right, 2 ],
    :* => [ :left, 3 ],
    :/ => [ :left, 3 ],
    :% => [ :left, 3 ],
    :+ => [ :left, 4 ],
    :- => [ :left, 4 ],
    :<< => [ :left, 5 ],
    :>> => [ :left, 5 ],
    :< => [ :left, 6 ],
    :<= => [ :left, 6 ],
    :> => [ :left, 6 ],
    :>= => [ :left, 6 ],
    :== => [ :left, 7 ],
    :!= => [ :left, 7 ],
    :& => [ :left, 8 ],
    :^ => [ :left, 9 ],
    :| => [ :left, 10 ],
    :"&&" => [ :left, 11 ],
    :"||" => [ :left, 12 ],
  }

  attr_reader :output

  def initialize
    @output = []
    @stack = []
  end

  def parse_expression(expression)
    scanner = StringScanner.new expression

    until scanner.eos?
      token = nil
      match = nil
      TOKENS.each do |(expression, tokenid)|
        result = scanner.scan expression
        if result
          token = tokenid
          match = result
          break
        end
      end

      next if token == :void

      if token.nil?
        raise "syntax error in #{expression.inspect}"
      end

      process_token token, match
    end

    finish

    self
  end

  private

  def finish
    until @stack.empty?
      token = @stack[-1]

      raise "mismatched parentheses" if token == :lparen

      @output.push token
      @stack.pop
    end
  end

  def operator_preceds(operator, operator_on_stack)
    if operator_on_stack == :lparen
      return false
    end

    op_assoc, op_predec = OPERATORS.fetch operator
    stack_op_assoc, stack_op_predec = OPERATORS.fetch operator_on_stack

    stack_op_predec < op_predec || (stack_op_predec == op_predec && stack_op_assoc == :left    )
  end

  def process_token(token, match)
    case token
    when :number
      @output.push Integer(match, 0)

    when :version
      match =~ /([0-9]+)\.([0-9]+)\.([0-9]+)\.([0-9]+)/
      @output.push ((Integer($1) & 0xFF) << 24) | ((Integer($2) & 0xFF) << 16) | ((Integer($3) & 0xFF) << 8) | (Integer($4) & 0xFF)

    when :variable
      @stack.push match

    when :lparen
      @stack.push :lparen

    when :operator
      operator = match.intern

      while !@stack.empty? && ((@stack[-1].is_a?(String)) || (operator_preceds(operator, @stack[-1])))
        @output.push @stack.pop
      end

      @stack.push operator

    when :rparen
      while !@stack.empty? && @stack.last != :lparen
        @output.push @stack.pop
      end

      raise "mismatched parentheses" if @stack.empty?

      @stack.pop
    end
  end
end
