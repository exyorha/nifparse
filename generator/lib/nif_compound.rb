class NIFCompound < NIFType
  attr_accessor :ver1, :ver2, :fields

  def initialize
    @fields = []
  end

  def encode_value_of_type(value, desc)
    values = value.split(",")
    values.map!(&:strip)

    unless values.size == @fields.size
      raise "number of default values does not match the number of fields"
    end

    data = "".force_encoding("BINARY")

    @fields.each_with_index do |field, index|
      data << desc.types.fetch(field.type).encode_value_of_type(values[index], desc)
    end

    data
  end
end
