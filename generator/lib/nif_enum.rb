class NIFEnum < NIFType
  attr_accessor :storage, :prefix, :options

  def initialize
    @options = []
  end

  def encode_value_of_type(value, desc)
    option = options.find do |option|
      option.name == value
    end

    unless option.nil?
      value = option.value
    end

    desc.types.fetch(storage).encode_value_of_type(value, desc)
  end
end
