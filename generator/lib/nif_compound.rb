class NIFCompound < NIFType
  attr_accessor :ver1, :ver2, :fields

  def initialize
    @fields = []
  end
end
