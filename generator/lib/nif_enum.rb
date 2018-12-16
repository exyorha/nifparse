class NIFEnum < NIFType
  attr_accessor :storage, :prefix, :options

  def initialize
    @options = []
  end
end
